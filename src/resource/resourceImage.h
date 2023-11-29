// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "resource/resource.h"
#include "common/texture.h"
#include "connector/withLogger.h"

enum class ByteMap
{
    None,
    Full,
    Alpha
};

class ResourceImage : public Resource, public WithLogger
{
public:
    EXPORT ResourceImage(std::string path, ByteMap byteMap, int byteMapScale);

    EXPORT Texture *getAsTexture();
    EXPORT void preload();

    EXPORT int getWidth();
    EXPORT int getHeight();

    EXPORT unsigned char getProbeBytemapAlpha(int x, int y);
    EXPORT unsigned int getProbeBytemapFull(int x, int y);

protected:
    void generateByteMap();

    Texture *texture = nullptr;

    int width = 0, height = 0, nrChannels = 0, byteMapScale;
    ByteMap byteMap = ByteMap::None;
    int mapWidth = 0, mapHeight = 0;

    unsigned char *bytemapData = nullptr;
};