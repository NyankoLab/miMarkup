// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#pragma once

#include <string>
#include <vector>

struct miMarkup : public std::vector<miMarkup>
{
    std::string name;
    std::string value;

    bool IsArray() const;
    bool IsBoolean() const;
    bool IsNull() const;
    bool IsNumber() const;
    bool IsObject() const;
    bool IsString() const;

    std::string toJSON() const;
    std::string toTOML() const;
    std::string toXML() const;
    std::string toYAML() const;

    static miMarkup* fromJSON(std::string_view json);
    static miMarkup* fromTOML(std::string_view toml);
    static miMarkup* fromXML(std::string_view xml);
    static miMarkup* fromYAML(std::string_view yaml);
};
