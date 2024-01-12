// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/renderTarget.h"
#include "connector/withLogger.h"
#include "controller/logController.h"
#include "renderer/renderer.h"
#include <string>

#define VIEW_CREATION_FLAG_NONE 0
#define VIEW_CREATION_FLAG_RESIZABLE 1
#define VIEW_CREATION_FLAG_NO_TOP_BAR 2

class View : public WithLogger
{
public:
    View(Config *config, unsigned int flags);

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
    inline bool isResizable() { return bIsResizable; }
    inline bool hasTopBar() { return bHasTopBar; }
    inline Renderer *getRenderer() { return renderer; }

    EXPORT void minimize();

    EXPORT float getDPIZoom();

    EXPORT RenderTarget *getRenderTarget();
    
    // called when resizable window is resized not for external use
    EXPORT void updateSize(int width, int height);

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

    bool bIsResizable = false;
    bool bHasTopBar = true;
};