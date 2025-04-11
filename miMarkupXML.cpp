//==============================================================================
// miMarkup : miMarkupXML Source
//
// Copyright (c) 2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#include "miMarkup.h"

static bool deserializeXML(std::string_view& xml, miMarkup& markup)
{
    while (xml.empty() == false) {
        char c = xml.front(); xml.remove_prefix(1);
        switch (c) {
        default:
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
            xml += child.name;
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
            xml += child.value;
        }
        if (child.name.empty() == false) {
            xml += '<';
            xml += '/';
            xml += child.name;
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
