// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "stage/stage.h"
#include <SDL.h>
#include <math.h>

Stage::Stage(std::string name)
{
    this->name = name;

    // debugLayer = new LayerDebug(debugLayerIndex);
    // layers.push_back(debugLayer);

    presentTrackerId = profiler->addTracker("stage \"" + name + "\" final render");
    buffersSwapTrackerId = profiler->addTracker("stage \"" + name + "\" buffer swap");
}

LayerActors *Stage::createLayerActors(std::string name, int index)
{
    auto newLayerActors = new LayerActors(name, index);
    layers.push_back(newLayerActors);
    sortLayers();
    return newLayerActors;
}

LayerEffects *Stage::createLayerEffects(std::string name, int index)
{
    auto newLayerEffects = new LayerEffects(name, index);
    layers.push_back(newLayerEffects);
    sortLayers();
    return newLayerEffects;
}

LayerUI *Stage::createLayerUI(std::string name, int index)
{
    auto newLayerUI = new LayerUI(name, index);
    layers.push_back(newLayerUI);
    sortLayers();
    return newLayerUI;
}

void Stage::process(float delta)
{
    for (auto it = layers.begin(); it != layers.end(); ++it)
    {
        (*it)->process(delta);
    }
}

void Stage::present(View *view)
{
    auto viewRenderTarget = view->getRenderTarget();
    present(view->getRenderer(), viewRenderTarget);

    profiler->startTracking(presentTrackerId);

    // present the framebuffer on the screen
    view->getRenderer()->presentToScreen(viewRenderTarget);

    profiler->stopTracking(presentTrackerId);

    profiler->startTracking(buffersSwapTrackerId);
    view->swapBuffers();
    profiler->stopTracking(buffersSwapTrackerId);
}

void Stage::present(Renderer *renderer, RenderTarget *renderTarget)
{
    profiler->startTracking(presentTrackerId);

    // set result buffer and clear it
    renderTarget->setupResultBuffer(true, Vector4(clearColor.r, clearColor.g, clearColor.b, 1.0f));
    profiler->stopTracking(presentTrackerId);

    for (auto it = layers.begin(); it != layers.end(); ++it)
    {
        if ((*it)->isVisible())
        {
            (*it)->render(renderer, renderTarget);
        }
    }
}

void Stage::sortLayers()
{
    layers.sort([](Layer *layer1, Layer *layer2)
                { return layer1->getIndex() < layer2->getIndex(); });
}
