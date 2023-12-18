// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/shader.h"
#include "renderer/effectBuffer.h"
#include "common/utils.h"
#include "stage/layer.h"
#include "connector/withProfiler.h"
#include "connector/withRenderer.h"
#include <vector>

class LayerEffects : public Layer, public WithProfiler, public WithRenderer
{
public:
    LayerEffects(std::string name, int index);
    ~LayerEffects();
    EXPORT void addEffect(Shader *effect, ShaderParameter **parameters = nullptr, int parametersAmount = 0);
    EXPORT void removeEffect(Shader *effect);
    EXPORT void clearEffects();
    EXPORT void render(Renderer *renderer, RenderTarget *renderTarget) override;

protected:
    std::vector<ParametredShader> effects;

    int renderTrackerId = 0;

    EffectBuffer *effectBuffer = nullptr;
};
