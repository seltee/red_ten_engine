// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/color.h"
#include "renderer/renderTarget.h"
#include "stage/layer.h"
#include "stage/layerActors.h"
#include "stage/layerEffects.h"
#include "stage/layerUI.h"
#include "stage/layerDebug.h"
#include "os/view.h"
#include "controller/debugController.h"
#include "connector/withProfiler.h"
#include <list>

class Stage : public WithProfiler, public WithDebug
{
public:
    Stage(std::string name);

    EXPORT LayerActors *createLayerActors(std::string name, int index);
    EXPORT LayerEffects *createLayerEffects(std::string name, int index);
    EXPORT LayerUI *createLayerUI(std::string name, int index);

    EXPORT void process(float delta);
    EXPORT void present(View *view);
    EXPORT void present(Renderer *renderer, RenderTarget *renderTarget);

    EXPORT void setClearColor(Color color)
    {
        clearColor = color;
    }

    const int debugLayerIndex = 9999;

protected:
    void sortLayers();

    RenderTarget *renderTarget = nullptr;
    std::string name;
    std::list<Layer *> layers;

    Color clearColor{0.15f, 0.15f, 0.15f};

    LayerDebug *debugLayer;

    int presentTrackerId = 0;
    int buffersSwapTrackerId = 0;
};
