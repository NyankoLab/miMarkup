//==============================================================================
// miMarkup : miMarkupJSON Source
//
// Copyright (c) 2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#include "miMarkup.h"

static bool deserializeJSON(std::string_view& json, miMarkup& markup)
{
    std::string* text = &markup.value;

    while (json.empty() == false) {
        char c = json.front(); json.remove_prefix(1);
        switch (c) {
        case ' ':
        case '\r':
        case '\n':
            break;
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
            (*text).clear();
            while (json.empty() == false) {
                c = json.front(); json.remove_prefix(1);
                if (c == '"') {
                    if ((*text).empty() == false || (*text).back() != '\\')
                        break;
                    (*text).pop_back();
                }
                (*text).push_back(c);
            }
            break;
        case ':':
            markup.name.swap(markup.value);
            text = &markup.value;
            break;
        default:
            (*text).push_back(c);
            break;
        }
    }

    return false;
}

static void serializeJSON(std::string& json, miMarkup const& markup, int depth = 0)
{
    auto indent = [&](int depth) {
        for (int i = 0; i < depth; ++i)
            json += ' ';
    };

    for (auto& child : markup) {
        if (&child != &markup.front())
            json += ',';
        json += '\n';
        indent(depth + 1);
        if (child.name.empty() == false) {
            json += '"';
            json += child.name;
            json += '"';
            json += ':';
        }
        if (child.IsArray()) {
            json += '[';
            serializeJSON(json, child, depth + 1);
            json += '\n';
            indent(depth + 1);
            json += ']';
            continue;
        }
        if (child.IsObject()) {
            json += '{';
            serializeJSON(json, child, depth + 1);
            json += '\n';
            indent(depth + 1);
            json += '}';
            continue;
        }
        if (child.IsString()) {
            json += '"';
            json += child.value;
            json += '"';
            continue;
        }
        json += child.value;
    }
}

miMarkup* miMarkupJSONLoad(std::string_view json)
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

std::string miMarkupJSONSave(miMarkup const& markup)
{
    std::string json;
    json += '{';
    serializeJSON(json, markup);
    json += '\n';
    json += '}';
    return json;
}
