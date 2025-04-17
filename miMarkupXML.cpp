// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#include "miMarkup.h"

static bool deserializeEscaping(std::string& text)
{
    auto it = text.rbegin();
    uint64_t value = 0;
    for (size_t i = 0; i < 64; i += 8) {
        if (it == text.rend())
            break;
        uint64_t c = uint8_t(*it++);
        if (c == '&')
            break;
        value |= c << i;
    }
    size_t size = text.size();
    switch (value) {
    case '\0\0lt':  // &lt;
        text.resize(size - 3);
        text.push_back('<');
        return true;
    case '\0\0gt':  // &gt;
        text.resize(size - 3);
        text.push_back('>');
        return true;
    case '\0amp':   // &amp;
        text.resize(size - 4);
        text.push_back('&');
        return true;
    case 'apos':  // &apos;
        text.resize(size - 5);
        text.push_back('\'');
        return true;
    case 'quot':  // &quot;
        text.resize(size - 5);
        text.push_back('\"');
        return true;
    }
    return false;
}

static std::string serializeEscaping(std::string const& text)
{
    std::string output;
    for (char c : text) {
        switch (c) {
        case '<':
            output += "&lt;";
            break;
        case '>':
            output += "&gt;";
            break;
        case '&':
            output += "&amp;";
            break;
        case '\'':
            output += "&apos;";
            break;
        case '\"':
            output += "&qout;";
            break;
        default:
            output += c;
        }
    }
    return output;
}

static bool deserializeXML(std::string_view& xml, miMarkup& markup)
{
    bool tag = false;
    bool slash = false;
    bool escape = false;
    std::string* text = &markup.value;
    std::string dummy;

    while (xml.empty() == false) {
        char c = xml.front(); xml.remove_prefix(1);
        switch (c) {
        case '<':
            tag = true;
            markup.push_back({});
            text = &markup.back().name;
            break;
        case '>':
            if (slash) {
                if (markup.empty())
                    return true;
                if (markup.back().name.empty())
                    markup.pop_back();
                if (markup.empty() == false)
                    markup.value.clear();
                if (markup.name == "array") {
                    if (markup.empty() == false || markup.value.empty() == false)
                        markup.name.clear();
                }
                return true;
            }
            tag = false;
            slash = false;
            text = &dummy;
            deserializeXML(xml, markup.back());
            break;
        case ';':
            if (escape) {
                escape = false;
                if (deserializeEscaping(*text))
                    continue;
            }
        case '/':
            if (tag) {
                slash = true;
                break;
            }
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            if (tag)
                break;
        default:
            if (c == '&')
                escape = true;
            if (slash)
                break;
            (*text).push_back(c);
            break;
        }
    }

    return false;
}

static void serializeXML(std::string& xml, miMarkup const& markup, size_t depth = 0, bool array = false)
{
    auto indent = [&](size_t space) {
        for (size_t i = 0; i < space; ++i)
            xml += ' ';
    };

    for (auto& child : markup) {
        indent(depth);
        if (array) {
            xml += "<array>";
        }
        if (child.name.empty() == false) {
            xml += '<';
            xml += serializeEscaping(child.name);
            xml += '>';
        }
        if (child.IsArray()) {
            xml += '\n';
            serializeXML(xml, child, depth + 1, true);
            indent(depth);
        }
        else if (child.IsObject()) {
            xml += '\n';
            serializeXML(xml, child, depth + 1);
            indent(depth);
        }
        else {
            xml += serializeEscaping(child.value);
        }
        if (child.name.empty() == false) {
            xml += '<';
            xml += '/';
            xml += serializeEscaping(child.name);
            xml += '>';
        }
        if (array) {
            xml += "</array>";
        }
        xml += '\n';
    }
}

miMarkup* miMarkup::fromXML(std::string_view xml)
{
    miMarkup* markup = new miMarkup;
    if (markup) {
        while (xml.empty() == false) {
            deserializeXML(xml, *markup);
        }
        if (markup->empty() && markup->name.empty() && markup->value.empty()) {
            delete markup;
            markup = nullptr;
        }
    }
    return markup;
}

std::string miMarkup::toXML() const
{
    std::string xml;
    serializeXML(xml, *this);
    return xml;
}
