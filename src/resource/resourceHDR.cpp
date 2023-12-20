// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/resourceHDR.h"
#include "loaders/stb_image.h"
#include "renderer/renderer.h"

ResourceHDR::ResourceHDR(std::string path, float ldrScale, float ldrGamma) : Resource(path)
{
    this->ldrScale = ldrScale;
    this->ldrGamma = ldrGamma;
}

Texture *ResourceHDR::getAsTexture()
{
    if (ambientTexture)
        return ambientTexture;

    stbi_hdr_to_ldr_gamma(ldrGamma);
    stbi_hdr_to_ldr_scale(ldrScale);

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

    if (data)
    {
        // TODO: Blur through shader
        // This code is not very fast
        // TODO: Process in threads at least
        int widthBlured = 128;
        int heightBlured = 128;
        float ratioWidth = static_cast<float>(width) / static_cast<float>(widthBlured);
        float ratioHeight = static_cast<float>(height) / static_cast<float>(heightBlured);
        unsigned char *rData = (unsigned char *)malloc(widthBlured * heightBlured * 4);

        for (int iy = 0; iy < heightBlured; iy++)
        {
            for (int ix = 0; ix < widthBlured; ix++)
            {
                int shift = (ix + iy * widthBlured) * 4;
                int originalX = static_cast<int>(static_cast<float>(ix) * ratioWidth) % width;
                int originalY = static_cast<int>(static_cast<float>(iy) * ratioHeight) % height;
                int originalShift = (originalX + originalY * width) * 4;

                rData[shift + 0] = data[originalShift + 0];
                rData[shift + 1] = data[originalShift + 1];
                rData[shift + 2] = data[originalShift + 2];
                rData[shift + 3] = 255;
            }
        }

        unsigned char *blurData = (unsigned char *)malloc(widthBlured * heightBlured * 4);
        int pickSize = 16;
        int pickSize2 = (pickSize + pickSize + 1) * (pickSize + pickSize + 1);

        for (int iy = 0; iy < heightBlured; iy++)
        {
            for (int ix = 0; ix < widthBlured; ix++)
            {
                unsigned int mData[3] = {0, 0, 0};
                int sShift = ix + iy * widthBlured;

                for (int pickY = -pickSize; pickY <= pickSize; pickY++)
                {
                    int linePick = (iy + pickY);
                    if (linePick >= heightBlured)
                        linePick = heightBlured - 1;
                    if (linePick < 0)
                        linePick = 0;

                    int linePickShift = linePick * widthBlured;

                    for (int pickX = -pickSize; pickX <= pickSize; pickX++)
                    {
                        int pixelPick = ix + pickX;
                        if (pixelPick < 0)
                            pixelPick += widthBlured;
                        if (pixelPick >= widthBlured)
                            pixelPick -= widthBlured;
                        int fullShift = ((linePickShift) + pixelPick) * 4;
                        mData[0] += rData[fullShift];
                        mData[1] += rData[fullShift + 1];
                        mData[2] += rData[fullShift + 2];
                    }
                }
                mData[0] /= pickSize2;
                mData[1] /= pickSize2;
                mData[2] /= pickSize2;
                blurData[sShift * 4] = mData[0];
                blurData[sShift * 4 + 1] = mData[1];
                blurData[sShift * 4 + 2] = mData[2];
                blurData[sShift * 4 + 3] = 255;
            }
        }

        ambientTexture = getRenderer()->createTexture(width, height, 4, data, false);
        redianceTexture = getRenderer()->createTexture(widthBlured, heightBlured, 4, blurData, false);

        stbi_image_free(data);
        free(rData);
        free(blurData);
    }
    else
    {
        logger->logff("ERROR: %s couldn't be loaded\n", path.c_str());
    }
    return ambientTexture;
}

Texture *ResourceHDR::getAsRadianceTexture()
{
    getAsTexture();
    return redianceTexture;
}

void ResourceHDR::preload()
{
    getAsTexture();
}

int ResourceHDR::getWidth()
{
    if (!width)
        getAsTexture();
    return width;
}

int ResourceHDR::getHeight()
{
    if (!height)
        getAsTexture();
    return height;
}
