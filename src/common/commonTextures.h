// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/texture.h"

class CommonTextures : public WithLogger
{
public:
    void build();

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
