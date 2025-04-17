// SPDX-FileCopyrightText: Copyright 2025 TAiGA
// SPDX-License-Identifier: MIT
// https://github.com/metarutaiga/miMarkup
#include "miMarkup.h"

bool miMarkup::IsArray() const
{
    if (empty())
        return false;
    if (front().name.empty() == false)
        return false;
    if (front().value.empty() && front().empty())
        return false;
    return true;
}

bool miMarkup::IsBoolean() const
{
    if (empty() == false)
        return false;
    if (strcasecmp(value.c_str(), "false") == 0)
        return true;
    if (strcasecmp(value.c_str(), "true") == 0)
        return true;
    return false;
}

bool miMarkup::IsNull() const
{
    if (empty() == false)
        return false;
    if (value.empty())
        return true;
    if (strcasecmp(value.c_str(), "null") == 0)
        return true;
    return false;
}

bool miMarkup::IsNumber() const
{
    if (empty() == false)
        return false;
    int dot = 0;
    for (auto it = value.begin(); it != value.end(); ++it) {
        char c = (*it);
        if (c == '-' && it == value.begin())
            continue;
        if (c >= '0' && c <= '9')
            continue;
        if (c == '.' && (dot++) == 0)
            continue;
        return false;
    }
    return true;
}

bool miMarkup::IsObject() const
{
    if (empty())
        return false;
    if (front().name.empty())
        return false;
    return true;
}

bool miMarkup::IsString() const
{
    if (IsBoolean())
        return false;
    if (IsNull())
        return false;
    if (IsNumber())
        return false;
    return true;
}
