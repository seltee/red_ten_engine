// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
#include <vector>
#include "common/utils.h"
#include "connector/withLogger.h"

enum class TextureFilter
{
    Linear,
    Nearest
};

class Texture : public WithLogger
{
protected:
    Texture();

public:
    EXPORT virtual ~Texture();

    EXPORT virtual void setFiltering(TextureFilter filter);
    inline TextureFilter getFiltering() { return filter; };

    inline int getWidth() { return width; }
    inline int getHeight() { return height; }

protected:
    TextureFilter filter = TextureFilter::Linear;

    int width = 0, height = 0, nrChannels = 0;
    unsigned int textureID = 0;
};