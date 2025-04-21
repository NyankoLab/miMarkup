// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#include "miMarkup.h"

static void deserializeYAML(std::string_view& yaml, miMarkup& markup, int const indent = -1, int space = 0)
{
    if (indent == -1) {
        if (markup.empty() || yaml.front() != ' ')
            markup.push_back({});
        deserializeYAML(yaml, markup.back(), 0);
        return;
    }

    bool array = false;
    bool child = false;
    std::string* text = &markup.value;

    while (yaml.empty() == false) {
        char c = yaml.front(); yaml.remove_prefix(1);
        switch (c) {
        case ':':
            markup.name.swap(markup.value);
            text = &markup.value;
            space = -1;
            break;
        case '\t':
        case ' ':
        case '-':
            if ((*text).empty()) {
                if (space != -1) {
                    space++;
                    child = true;
                    if (c == '-')
                        array = true;
                }
                break;
            }
            (*text).push_back(c);
            break;
        case '\n':
        case '\r':
            return;
        default:
            if (child) {
                child = false;
                if (array) {
                    array = false;
                    markup.push_back({});
                    markup.back().push_back({});
                }
                else if (markup.empty() == false && markup.back().empty() == false) {
                    markup.back().push_back({});
                }
                else {
                    markup.push_back({});
                }
            }
            if (markup.empty())
                space = indent;
            if (indent != space && space > 0) {
                yaml = std::string_view(yaml.data() - 1, yaml.size() + 1);
                if (indent < space) {
                    deserializeYAML(yaml, markup.back(), indent + 1, space);
                    if (markup.empty() == false && markup.name.empty() && markup.value.empty() &&
                        markup.back().empty() && markup.back().name.empty() && markup.back().value.empty() == false) {
                        markup.value.swap(markup.back().value);
                        markup.pop_back();
                    }
                }
                return;
            }
            (*text).push_back(c);
            break;
        }
    }
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
            deserializeYAML(yaml, *markup);
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
