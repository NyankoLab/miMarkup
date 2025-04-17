// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#include "miMarkup.h"

static char deserializeEscaping(char c)
{
    switch (c) {
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    }
    return c;
}

static std::string serializeEscaping(std::string const& text)
{
    std::string output;
    for (char c : text) {
        switch (c) {
        case '\b':
            output += "\\b";
            break;
        case '\f':
            output += "\\f";
            break;
        case '\n':
            output += "\\n";
            break;
        case '\r':
            output += "\\r";
            break;
        case '\t':
            output += "\\t";
            break;
        default:
            output += c;
        }
    }
    return output;
}

static bool deserializeJSON(std::string_view& json, miMarkup& markup)
{
    bool escape = false;
    std::string* text = &markup.value;

    while (json.empty() == false) {
        char c = json.front(); json.remove_prefix(1);
        switch (c) {
        case '[':
        case '{':
            do {
                markup.push_back({});
            } while (deserializeJSON(json, markup.back()));
            break;
        case ',':
            return true;
        case ']':
        case '}':
            return false;
        case '"':
            escape = false;
            (*text).clear();
            while (json.empty() == false) {
                c = json.front(); json.remove_prefix(1);
                if (c == '"' && escape == false)
                    break;
                if ((*text).empty() == false && escape) {
                    (*text).pop_back();
                    c = deserializeEscaping(c);
                }
                escape = (c == '\\') && (escape == false);
                (*text).push_back(c);
            }
            break;
        case ':':
            markup.name.swap(markup.value);
            text = &markup.value;
            break;
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            break;
        default:
            (*text).push_back(c);
            break;
        }
    }

    return false;
}

static void serializeJSON(std::string& json, miMarkup const& markup, size_t depth = 0, bool array = false)
{
    auto indent = [&](size_t space) {
        for (size_t i = 0; i < space; ++i)
            json += ' ';
    };

    json += array ? '[' : '{';

    for (auto& child : markup) {
        if (&child != &markup.front())
            json += ',';
        json += '\n';
        indent(depth + 1);
        if (child.name.empty() == false) {
            json += '"';
            json += serializeEscaping(child.name);
            json += '"';
            json += ':';
        }
        if (child.IsArray()) {
            serializeJSON(json, child, depth + 1, true);
            continue;
        }
        if (child.IsObject()) {
            serializeJSON(json, child, depth + 1);
            continue;
        }
        if (child.IsString()) {
            json += '"';
            json += serializeEscaping(child.value);
            json += '"';
            continue;
        }
        json += child.value;
    }

    json += '\n';
    indent(depth);
    json += array ? ']' : '}';
}

miMarkup* miMarkup::fromJSON(std::string_view json)
{
    miMarkup* markup = new miMarkup;
    if (markup) {
        while (json.empty() == false) {
            deserializeJSON(json, *markup);
        }
        if (markup->empty() && markup->name.empty() && markup->value.empty()) {
            delete markup;
            markup = nullptr;
        }
    }
    return markup;
}

std::string miMarkup::toJSON() const
{
    std::string json;
    serializeJSON(json, *this);
    return json;
}
