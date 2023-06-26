// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
#include <vector>
#include "common/utils.h"
#include "common/withLogger.h"

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

enum class TextureFilter
{
    Linear,
    Nearest
};

class Texture : public WithLogger
{
public:
    EXPORT Texture(std::string path);
    EXPORT ~Texture();

    EXPORT void bind();
    EXPORT void bind(TextureSlot slot);

    EXPORT void reload();
    EXPORT void unload();
    EXPORT void clear();
    EXPORT bool isLoaded();
    EXPORT bool isPath(std::string path);

    EXPORT unsigned int getGLTextureId();
    EXPORT int getWidth();
    EXPORT int getHeight();

    EXPORT void processBytemaps(const unsigned char *data = nullptr, int width = 0, int height = 0, int nrChannels = 0);
    EXPORT void setMakeBytemapAlpha(bool state);
    EXPORT void setMakeBytemapFull(bool state);
    EXPORT void setBytemapScale(int scale);
    EXPORT unsigned char getProbeBytemapAlpha(int x, int y);
    EXPORT unsigned int getProbeFull(int x, int y);
    EXPORT const unsigned char *getBytemapAlphaData();
    EXPORT const unsigned char *getBytemapFullData();

    EXPORT void setTextureFilter(TextureFilter filter);
    EXPORT TextureFilter getTextureFilter();

protected:
    TextureFilter filter = TextureFilter::Linear;

    std::string path;
    std::vector<Texture *> list;

    int width = 0, height = 0, nrChannels = 0;
    unsigned int textureID = 0;
    bool bIsLoaded = false;

    bool bMakeBytemapAlpha = false;
    bool bMakeFullBytemap = false;
    int byteMapScale = 1;
    int mapWidth = 0, mapHeight = 0;
    unsigned char *bytemapFullData = nullptr;
    unsigned char *bytemapAlphaData = nullptr;
};