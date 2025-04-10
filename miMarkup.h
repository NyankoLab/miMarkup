//==============================================================================
// miMarkup : miMarkup Header
//
// Copyright (c) 2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
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
};

miMarkup* miMarkupJSONLoad(std::string_view json);
std::string miMarkupJSONSave(miMarkup const& markup);
