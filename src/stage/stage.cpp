// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "common/commonShaders.h"
#include "stage/stage.h"
#include "opengl/glew.h"
#include <SDL.h>
#include <math.h>

Stage::Stage(std::string name)
{
    this->name = name;

    debugLayer = new LayerDebug(debugLayerIndex);
    layers.push_back(debugLayer);
    sortLayers();

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
    present(viewRenderTarget);

    // present the framebuffer on the screen
    Matrix4 m;
    profiler->startTracking(presentTrackerId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    auto screenShader = CommonShaders::getScreenShader();
    screenShader->use(m, m);

    glBindTexture(GL_TEXTURE_2D, viewRenderTarget->getResultTexture());

    CommonShaders::getScreenMesh()->use();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    profiler->stopTracking(presentTrackerId);

    profiler->startTracking(buffersSwapTrackerId);
    view->swapBuffers();
    profiler->stopTracking(buffersSwapTrackerId);
}

void Stage::present(RenderTarget *renderTarget)
{
    profiler->startTracking(presentTrackerId);
    renderTarget->useResultBuffer();
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    profiler->stopTracking(presentTrackerId);

    for (auto it = layers.begin(); it != layers.end(); ++it)
    {
        if ((*it)->isVisible())
        {
            (*it)->render(renderTarget);
        }
    }
}

void Stage::sortLayers()
{
    layers.sort([](Layer *layer1, Layer *layer2)
                { return layer1->getIndex() < layer2->getIndex(); });
}
