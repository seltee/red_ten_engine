// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/texture.h"

enum class TextureSlot
{
    TEXTURE_0 = 0x84C0,
    TEXTURE_1 = 0x84C1,
    TEXTURE_2 = 0x84C2,
    TEXTURE_3 = 0x84C3,
    TEXTURE_4 = 0x84C4,
    TEXTURE_5 = 0x84C5,
    TEXTURE_6 = 0x84C6,
    TEXTURE_7 = 0x84C7,
    TEXTURE_8 = 0x84C8,
    TEXTURE_9 = 0x84C9,
    TEXTURE_10 = 0x84CA,
    TEXTURE_11 = 0x84CB,
    TEXTURE_12 = 0x84CC,
    TEXTURE_13 = 0x84CD,
    TEXTURE_14 = 0x84CE,
    TEXTURE_15 = 0x84CF,
    TEXTURE_16 = 0x84D0,
    TEXTURE_17 = 0x84D1,
    TEXTURE_18 = 0x84D2,
    TEXTURE_19 = 0x84D3,
    TEXTURE_20 = 0x84D4,
    TEXTURE_21 = 0x84D5,
    TEXTURE_22 = 0x84D6,
    TEXTURE_23 = 0x84D7,
    TEXTURE_24 = 0x84D8,
    TEXTURE_25 = 0x84D9,
    TEXTURE_26 = 0x84DA,
    TEXTURE_27 = 0x84DB,
    TEXTURE_28 = 0x84DC,
    TEXTURE_29 = 0x84DD,
    TEXTURE_30 = 0x84DE,
    TEXTURE_31 = 0x84DF
};

class TextureOpengGL : public Texture
{
public:
    EXPORT TextureOpengGL(unsigned int textureID);
    EXPORT TextureOpengGL(int width, int height);
    EXPORT ~TextureOpengGL();

    EXPORT void bind();
    EXPORT void bind(TextureSlot slot);

    EXPORT unsigned int getGLTextureId();

    EXPORT void setFiltering(TextureFilter filter) final override;

    EXPORT Texture *clone() final override;

protected:
    unsigned int textureID = 0;
};