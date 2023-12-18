// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/resourceImage.h"
#include "loaders/stb_image.h"
#include "renderer/renderer.h"

ResourceImage::ResourceImage(std::string path, ByteMap byteMap, int byteMapScale) : Resource(path)
{
    this->byteMap = byteMap;
    this->byteMapScale = byteMapScale;

    if (byteMap != ByteMap::None)
        generateByteMap();
}

Texture *ResourceImage::getAsTexture()
{
    if (texture)
        return texture;

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
    if (data)
    {
        logger->logf("Image `%s` loaded", path.c_str());
        texture = getRenderer()->createTexture(width, height, 4, data);
        stbi_image_free(data);
    }
    else
    {
        logger->logff("ERROR: %s couldn't be loaded\n", path.c_str());
    }
    return texture;
}

void ResourceImage::preload()
{
    getAsTexture();
}

int ResourceImage::getWidth()
{
    if (!width)
        getAsTexture();
    return width;
}

int ResourceImage::getHeight()
{
    if (!height)
        getAsTexture();
    return height;
}

void ResourceImage::generateByteMap()
{
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
    if (data && width && height && nrChannels)
    {
        mapWidth = width / byteMapScale;
        mapHeight = height / byteMapScale;

        int byteMapSize = 0;
        switch (byteMap)
        {
        case ByteMap::Alpha:
            byteMapSize = mapWidth * mapHeight;
            break;

        case ByteMap::Full:
            byteMapSize = mapWidth * mapHeight * 4;
            break;

        default:
            byteMapSize = 0;
        }

        if (byteMapSize)
            bytemapData = (unsigned char *)malloc(byteMapSize);

        if (bytemapData)
        {
            int sourceSize = width * height * 4;
            for (int iy = 0; iy < mapHeight; iy++)
            {
                int shiftY = iy * mapWidth;
                int sourceShiftY = iy * width * byteMapScale;
                for (int ix = 0; ix < mapWidth; ix++)
                {
                    int sourceAddr = ((sourceShiftY + (ix * byteMapScale)) << 2) + 3;

                    switch (byteMap)
                    {
                    case ByteMap::Alpha:
                        bytemapData[shiftY + ix] = sourceAddr < sourceSize ? data[sourceAddr] : 0;
                        break;

                    case ByteMap::Full:
                        reinterpret_cast<int *>(bytemapData)[(shiftY + ix) << 2] = sourceAddr < sourceSize ? *((int *)(&data[sourceAddr])) : 0;
                        break;
                    default:
                        memset(bytemapData, 0, byteMapSize);
                        break;
                    }
                }
            }
        }
    }
    if (data)
    {
        stbi_image_free(data);
    }
}

unsigned char ResourceImage::getProbeBytemapAlpha(int x, int y)
{
    x /= byteMapScale;
    y /= byteMapScale;
    y = (mapHeight - y - 1);
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
    {
        switch (byteMap)
        {
        case ByteMap::Alpha:
            return bytemapData[y * mapWidth + x];
        case ByteMap::Full:
            return bytemapData[((y * mapWidth + x) << 2) + 3];
        default:
            return 0;
        }
    }
    return 0;
}

unsigned int ResourceImage::getProbeBytemapFull(int x, int y)
{
    x /= byteMapScale;
    y /= byteMapScale;
    y = (mapHeight - y - 1);
    char c = 0;
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
    {
        switch (byteMap)
        {
        case ByteMap::Alpha:
            c = bytemapData[y * mapWidth + x];
            return c + (c << 8) + (c << 16) + (c << 24);
        case ByteMap::Full:
            return reinterpret_cast<int *>(bytemapData)[y * mapWidth + x];
        default:
            return 0;
        }
    }
    return 0;
}