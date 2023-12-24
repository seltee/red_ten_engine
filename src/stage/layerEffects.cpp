// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "stage/layerEffects.h"
#include "renderer/renderer.h"

LayerEffects::LayerEffects(std::string name, int index) : Layer(name, index)
{
    effectBuffer = getRenderer()->createEffectBuffer();
    renderTrackerId = profiler->addTracker("layer effects \"" + name + "\" render");
}

LayerEffects::~LayerEffects()
{
    getRenderer()->destroyEffectBuffer(effectBuffer);
}

void LayerEffects::addEffect(Shader *effect, ShaderParameter **parameters, int parametersAmount)
{
    effects.push_back(ParametredShader({effect, parameters, parametersAmount}));
}

void LayerEffects::removeEffect(Shader *effect)
{
    auto it = effects.begin();
    while (it != effects.end())
        if (it->shader == effect)
            it = effects.erase(it);
        else
            ++it;
}

void LayerEffects::clearEffects()
{
    effects.clear();
}

void LayerEffects::render(Renderer *renderer, RenderTarget *renderTarget)
{
    if (!effectBuffer)
        return;
    profiler->startTracking(renderTrackerId);

    if (!effects.empty())
    {
        int width = renderTarget->getWidth();
        int height = renderTarget->getHeight();
        effectBuffer->prepare(width, height);

        for (auto it = effects.begin(); it != effects.end(); it++)
        {
            auto effect = *it;

            if (effect.shader->isReady() && effect.shader->getOpacity() > 0.001f)
            {
                effectBuffer->render(renderer, renderTarget, &effect);
            }
        }
    }
    profiler->stopTracking(renderTrackerId);
}