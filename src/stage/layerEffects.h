// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "shaders/effect.h"
#include "stage/layer.h"
#include "connector/withProfiler.h"
#include <vector>

class LayerEffects : public Layer, public WithProfiler
{
public:
    LayerEffects(std::string name, int index);
    EXPORT void addEffect(Effect *effect);
    EXPORT void removeEffect(Effect *effect);
    EXPORT void clearEffects();
    EXPORT void process(float delta);
    EXPORT void render(RenderTarget *renderTarget);

protected:
    std::vector<Effect *> effects;
    unsigned int framebuffer = 0;
    unsigned int renderedTexture = 0;

    int processTrackerId = 0;
    int renderTrackerId = 0;
};
