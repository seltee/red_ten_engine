// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/config.h"
#include "renderer/texture.h"
#include "renderer/renderer.h"

class RenderTarget
{
public:
    RenderTarget(int width, int height, RenderQuality quality, float multiSampling);
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
    void setupResultBuffer(bool clear = true, Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f));
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
    int resultWidth, resultHeight;
    int shadowMapSize;

    Texture *resultTextureAsClass = nullptr;
    float multiSampling = 1.0f;
};