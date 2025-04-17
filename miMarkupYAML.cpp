// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#include "miMarkup.h"

static void deserializeYAML(std::string_view& yaml, miMarkup& markup, int const indent = 0) __attribute__((optnone))
{
    int space = indent;
    std::string dummy;
    std::string* text = &markup.value;
    std::string_view backup = yaml;

    while (yaml.empty() == false) {
        char c = yaml.front(); yaml.remove_prefix(1);
        switch (c) {
        case ':':
            markup.name.swap(markup.value);
            text = &markup.value;
            break;
        case '\t':
        case ' ':
        case '-':
            if ((*text).empty()) {
                if (markup.name.empty() || markup.empty() == false) {
                    if (space == -1)
                        space = 0;
                    space++;
                }
                break;
            }
            (*text).push_back(c);
            break;
        case '\n':
        case '\r':
            space = -1;
            break;
        default:
            if (indent != space) {
                yaml = backup;
                if (indent < space) {
                    markup.push_back({});
                    deserializeYAML(yaml, markup.back(), space);
                    space = 0;
                    break;
                }
                if (indent > space) {
                    return;
                }
            }
            (*text).push_back(c);
            break;
        }
        backup = yaml;
    }

    return;
}

static void serializeYAML(std::string& yaml, miMarkup const& markup, size_t depth = 0, bool array = false)
{
    auto indent = [&](size_t space) {
        for (size_t i = 0; i < space; ++i)
            yaml += ' ';
    };

    size_t length = 0;
    for (auto& child : markup) {
        if (child.value.empty())
            continue;
        length = std::max(length, child.name.size());
    }

    if (yaml.empty() == false && yaml.back() != ' ')
        yaml += '\n';

    for (auto& child : markup) {
        if (&child != &markup.front())
            yaml += '\n';
        if (yaml.empty() == false && yaml.back() != ' ')
            indent(depth);
        if (array) {
            yaml += '-';
            yaml += ' ';
        }
        if (child.name.empty() == false) {
            yaml += child.name;
            yaml += ':';
        }
        if (child.IsArray()) {
            serializeYAML(yaml, child, depth + 1, true);
            continue;
        }
        if (child.IsObject()) {
            serializeYAML(yaml, child, depth + (array ? 2 : 1));
            continue;
        }
        if (child.name.empty() == false && child.value.empty() == false)
            indent(length - child.name.size() + 1);
        yaml += child.value;
    }
}

miMarkup* miMarkup::fromYAML(std::string_view yaml)
{
    miMarkup* markup = new miMarkup;
    if (markup) {
        while (yaml.empty() == false) {
            markup->push_back({});
            deserializeYAML(yaml, (*markup).back());
        }
        if (markup->empty() && markup->name.empty() && markup->value.empty()) {
            delete markup;
            markup = nullptr;
        }
    }
    return markup;
}

std::string miMarkup::toYAML() const
{
    std::string yaml;
    serializeYAML(yaml, *this);
    return yaml;
}
