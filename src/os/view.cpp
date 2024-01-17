// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "os/view.h"
#include "renderer/opengl/rendererOpenGL.h"
#include "renderer/vulkan/rendererVulkan.h"
#include "connector/withRenderer.h"
#include <SDL.h>

View::View(Config *config, unsigned int flags)
{
    this->config = config;
    this->width = config->getWindowWidth();
    this->height = config->getWindowHeight();
    this->refreshRate = config->getRefreshRate();
    this->bIsFullscreen = config->isFullscreen();

    if (flags & VIEW_CREATION_FLAG_RESIZABLE)
        bIsResizable = true;
    if (flags & VIEW_CREATION_FLAG_NO_TOP_BAR)
        bHasTopBar = false;

    updateSuitableDisplayMode();
}

bool View::makeWindow()
{
    if (window)
        return false;

    if (!RendererOpenGL::isAvailable())
        return false;

    renderer = new RendererOpenGL(config);
    // renderer = new RendererVulkan();

    renderer->preInit();

    auto newWindow = SDL_CreateWindow(windowName.c_str(),
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      width, height,
                                      (bIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0) |
                                          SDL_WINDOW_ALLOW_HIGHDPI |
                                          renderer->getWindowFlags() |
                                          (bHasTopBar ? 0 : SDL_WINDOW_BORDERLESS) |
                                          (bIsResizable ? SDL_WINDOW_RESIZABLE : 0));

    if (newWindow)
    {
        if (bIsFullscreen && displayMode != -1)
        {
            SDL_DisplayMode mode;
            SDL_GetDisplayMode(0, displayMode, &mode);
            SDL_SetWindowDisplayMode(newWindow, &mode);
        }

        if (!renderer->init(newWindow))
        {
            delete renderer;
            SDL_DestroyWindow(newWindow);
            return false;
        }

        WithRenderer::setCurrentRenderer(renderer);
        window = newWindow;

        SDL_GL_GetDrawableSize((SDL_Window *)window, &drawableWidth, &drawableHeight);
        if (SDL_GL_SetSwapInterval(-1) == 0)
            logger->logff("Adaptive V-Sync enabled");
        else
        {
            if (SDL_GL_SetSwapInterval(1) == 0)
                logger->logff("V-Sync enabled");
            else
            {
                SDL_GL_SetSwapInterval(0);
                logger->logff("No frame sync enabled");
            }
        }

        updateFrameBuffer();

        return true;
    }

    window = nullptr;
    return false;
}

bool View::changeMode()
{
    bool needUpdate = getWidth() != config->getWindowWidth() ||
                      getHeight() != config->getWindowHeight() ||
                      getRefreshRate() != config->getRefreshRate() ||
                      isFullscreen() != config->isFullscreen();

    if (needUpdate)
    {
        this->width = config->getWindowWidth();
        this->height = config->getWindowHeight();
        this->refreshRate = config->getRefreshRate();
        bool isFullscreen = config->isFullscreen();
        updateSuitableDisplayMode();

        if (window)
        {
            if (displayMode == -1 && this->bIsFullscreen)
            {
                SDL_SetWindowFullscreen((SDL_Window *)window, 0);
                this->bIsFullscreen = false;
            }

            if (displayMode != -1)
            {
                this->bIsFullscreen = isFullscreen;
                SDL_DisplayMode mode;
                SDL_GetDisplayMode(0, displayMode, &mode);
                SDL_SetWindowDisplayMode((SDL_Window *)window, &mode);
                SDL_SetWindowSize((SDL_Window *)window, mode.w, mode.h);

                if (bIsFullscreen)
                    SDL_SetWindowFullscreen((SDL_Window *)window, SDL_WINDOW_FULLSCREEN);
                else
                    SDL_SetWindowFullscreen((SDL_Window *)window, 0);
            }
            else
            {
                if (this->bIsFullscreen)
                {
                    SDL_SetWindowFullscreen((SDL_Window *)window, 0);
                    this->bIsFullscreen = false;
                }
                else
                    SDL_SetWindowSize((SDL_Window *)window, width, height);
            }
        }
        else
        {
            this->bIsFullscreen = isFullscreen;
            return makeWindow();
        }
    }

    SDL_GL_GetDrawableSize((SDL_Window *)window, &drawableWidth, &drawableHeight);
    updateFrameBuffer();
    return true;
}

void View::swapBuffers()
{
    SDL_GL_SwapWindow((SDL_Window *)window);
}

void View::minimize()
{
    if (window)
        SDL_MinimizeWindow((SDL_Window *)window);
}

float View::getDPIZoom()
{
    if (window)
    {
        int displayIndex = SDL_GetWindowDisplayIndex((SDL_Window *)window);
        float ddpi, hdpi, vdpi;
        SDL_GetDisplayDPI(displayIndex, &ddpi, &hdpi, &vdpi);

        return ddpi / 92.0f;
    }
    return 1.0f;
}

RenderTarget *View::getRenderTarget()
{
    return renderTarget;
}

void View::updateSize(int width, int height)
{
    if (this->width != width || this->height != height)
    {
        this->width = width;
        this->height = height;

        SDL_GL_GetDrawableSize((SDL_Window *)window, &drawableWidth, &drawableHeight);
        updateFrameBuffer();
    }
}

void View::updateSuitableDisplayMode()
{
    displayMode = -1;
    int highestRefreshRate = 0;
    SDL_DisplayMode mode;
    int displayModes = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < displayModes; i++)
    {
        SDL_GetDisplayMode(0, i, &mode);
        if (mode.w == width && mode.h == height && ((refreshRate == 0 && mode.refresh_rate > highestRefreshRate) || mode.refresh_rate == refreshRate))
        {
            highestRefreshRate = mode.refresh_rate;
            displayMode = i;
        }
    }
}

void View::updateFrameBuffer()
{
    printf("Create render target %i %i\n", drawableWidth, drawableHeight);
    if (renderTarget)
        delete renderTarget;
    renderTarget = new RenderTarget(drawableWidth, drawableHeight, config->getShadowQuality(), config->getMultisamplingFactor());
}