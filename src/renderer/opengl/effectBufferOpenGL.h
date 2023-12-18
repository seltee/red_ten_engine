// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/effectBuffer.h"

class EffectBufferOpenGL : public EffectBuffer
{
public:
    EffectBufferOpenGL();
    ~EffectBufferOpenGL();
    void prepare(int width, int height) override;
    void render(Renderer *renderer, RenderTarget *renderTarget, ParametredShader *effect) override;

protected:
    int width = 0, height = 0;
    unsigned int framebuffer = -1;
    unsigned int renderedTexture = -1;
};