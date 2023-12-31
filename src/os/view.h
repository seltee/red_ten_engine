// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/renderTarget.h"
#include "connector/withLogger.h"
#include "controller/logController.h"
#include "renderer/renderer.h"
#include <string>

class View : public WithLogger
{
public:
    View(Config *config);

    bool makeWindow();
    bool changeMode();

    EXPORT void swapBuffers();

    inline int getWidth() { return width; }
    inline int getHeight() { return height; }
    inline int getDrawableWidth() { return drawableWidth; }
    inline int getDrawableHeight() { return drawableHeight; }
    inline int getRefreshRate() { return refreshRate; }
    inline float getHWProportion() { return (float)height / (float)width; }
    inline bool isFullscreen() { return bIsFullscreen; }

    inline Renderer *getRenderer() { return renderer; }

    EXPORT void minimize();

    EXPORT RenderTarget *getRenderTarget();

    std::string windowName = "unnamed";

protected:
    void updateSuitableDisplayMode();
    void updateFrameBuffer();

    void *window = nullptr;
    int width = 640;
    int height = 480;
    int drawableWidth = 640;
    int drawableHeight = 480;
    int refreshRate = 0;
    int displayMode = -1;
    bool bIsFullscreen = false;

    RenderTarget *renderTarget = nullptr;
    Config *config = nullptr;

    Renderer *renderer = nullptr;
};