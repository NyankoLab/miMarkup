// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#include "miMarkup.h"

static bool deserializeTOML(std::string_view& toml, miMarkup& markup)
{
    while (toml.empty() == false) {
        char c = toml.front(); toml.remove_prefix(1);
        switch (c) {
        default:
            break;
        }
    }

    return false;
}

static void serializeTOML(std::string& toml, miMarkup const& markup, std::string path = "", bool array = false)
{
    std::string label;
    for (size_t i = 0; i < 2; ++i) {
        for (auto& child : markup) {
            if (((child.IsArray() && child.front().IsObject()) || child.IsObject()) == (i == 0))
                continue;
            if (&child != &markup.front() && array)
                toml += ',';
            if (&child != &markup.front())
                toml += '\n';
            if (array)
                toml += ' ';
            if (child.IsArray() || child.IsObject()) {
                label = path;
                if (label.empty() == false && child.name.empty() == false)
                    label += '.';
                label += child.name;
            }
            if (child.IsArray()) {
                if (child.front().IsObject()) {
                    serializeTOML(toml, child, label);
                    continue;
                }
                toml += label;
                toml += '=';
                toml += '[';
                toml += '\n';
                serializeTOML(toml, child, label, true);
                toml += '\n';
                toml += ']';
                continue;
            }
            if (child.IsObject()) {
                toml += '\n';
                toml += '[';
                toml += label;
                toml += ']';
                toml += '\n';
                serializeTOML(toml, child, label);
                continue;
            }
            if (child.name.empty() == false) {
                toml += child.name;
                toml += '=';
            }
            if (child.IsString()) {
                toml += '"';
                toml += child.value;
                toml += '"';
                continue;
            }
            toml += child.value;
        }
    }
}

miMarkup* miMarkup::fromTOML(std::string_view toml)
{
    miMarkup* markup = new miMarkup;
    if (markup) {
        while (toml.empty() == false) {
            deserializeTOML(toml, *markup);
        }
        if (markup->empty() && markup->name.empty() && markup->value.empty()) {
            delete markup;
            markup = nullptr;
        }
    }
    return markup;
}

std::string miMarkup::toTOML() const
{
    std::string toml;
    serializeTOML(toml, *this);
    return toml;
}
