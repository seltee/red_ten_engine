// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/texture.h"

class Renderer;

class CommonTextures : public WithLogger
{
public:
    void build(Renderer *renderer);

    static Texture *getBlackTexture();
    static Texture *getWhiteTexture();
    static Texture *getGreyTexture();
    static Texture *getZeroNormalTexture();

protected:
    static Texture *blackTexture;
    static Texture *whiteTexture;
    static Texture *greyTexture;
    static Texture *zeroNormalTexture;
};
