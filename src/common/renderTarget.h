// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/config.h"
#include "common/texture.h"

class RenderTarget
{
public:
    RenderTarget(int width, int height, RenderQuality quality);
    ~RenderTarget();

    inline int getWidth() { return width; }
    inline int getHeight() { return height; }

    unsigned int getPositionTexture();
    unsigned int getNormalTexture();
    unsigned int getAlbedoTexture();
    unsigned int getLightningTexture();
    unsigned int getShadowTexture();
    unsigned int getResultTexture();

    Texture *getResultTextureAsClass() { return resultTextureAsClass; }

    void useResultBuffer();

    void setupNewFrame(bool clear = true);
    void setupLightning(bool clear = true);
    void setupShadowHQ(bool clear = true);
    int getShadowMapSize();

protected:
    unsigned int gBuffer, depthbuffer;
    unsigned int gPosition, gNormal, gAlbedoSpec;

    unsigned int lightningBuffer;
    unsigned int lightningTexture;

    unsigned int shadowBuffer;
    unsigned int shadowTexture;

    unsigned int resultBuffer;
    unsigned int resultTexture;

    int width, height;
    int shadowMapSize;

    Texture *resultTextureAsClass = nullptr;
};