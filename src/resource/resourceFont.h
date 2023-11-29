// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
#include "resource/resource.h"
#include "common/utils.h"
#include "static/dm_sans.h"

struct InternalFont
{
};

class ResourceFont : public Resource
{
public:
    ResourceFont(std::string path, int size);
    int getSize();
    InternalFont *getFont();

protected:
    int size;
    InternalFont *font;
};
