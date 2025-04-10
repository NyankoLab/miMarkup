//==============================================================================
// miMarkup : Markup Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/miMarkup
//==============================================================================
#pragma once

struct Markup
{
    static void Initialize();
    static void Shutdown();
    static bool Update(const UpdateData& updateData, bool& show);
};
