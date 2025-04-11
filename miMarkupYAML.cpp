//==============================================================================
// miMarkup : miMarkupYAML Source
//
// Copyright (c) 2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#include "miMarkup.h"

static bool deserializeYAML(std::string_view& yaml, miMarkup& markup)
{
    while (yaml.empty() == false) {
        char c = yaml.front(); yaml.remove_prefix(1);
        switch (c) {
        default:
            break;
        }
    }

    return false;
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
        if (child.name.empty() == false) {
            indent(length - child.name.size() + 1);
        }
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
