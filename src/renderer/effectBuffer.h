// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/renderTarget.h"
#include "renderer/shader.h"

struct ParametredShader
{
    Shader *shader;
    ShaderParameter **parameters;
    int parametersAmount;
};

class EffectBuffer
{
public:
    virtual ~EffectBuffer();
    virtual void prepare(int width, int height);
    virtual void render(Renderer *renderer, RenderTarget *target, ParametredShader *shader);
};