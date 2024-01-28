// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
#include <vector>
#include "common/utils.h"
#include "connector/withLogger.h"
#include "math/math.h"

class Shader;

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
    EXPORT virtual Texture *clone();

    EXPORT virtual bool isEditable();

    inline TextureFilter getFiltering() { return filter; };

    inline int getWidth() { return width; }
    inline int getHeight() { return height; }

    EXPORT virtual void drawImage(Texture *texture, Vector2 position);
    EXPORT virtual void drawImage(Texture *texture, Vector2 position, Vector2 Scale);
    EXPORT virtual void drawImage(Texture *texture, Vector2 position, Vector2 Scale, Vector2 alignPoint, float rotation);
    EXPORT virtual void drawImage(Texture *texture, Vector2 position, Vector2 Scale, float rotation);
    EXPORT virtual void drawImage(Texture *texture, Matrix4 *model, Shader *shader);
    EXPORT virtual void recreateMipMaps();

protected:
    TextureFilter filter = TextureFilter::Linear;

    int width = 0, height = 0, nrChannels = 0;
};