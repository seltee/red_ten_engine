// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "os/view.h"
#include "opengl/glew.h"
#include "common/commonShaders.h"
#include <SDL.h>

View::View(Config *config)
{
    this->config = config;
    this->width = config->getWindowWidth();
    this->height = config->getWindowHeight();
    this->refreshRate = config->getRefreshRate();
    this->bIsFullscreen = config->isFullscreen();
    updateSuitableDisplayMode();
}

bool View::makeWindow()
{
    if (window)
        return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    auto newWindow = SDL_CreateWindow(windowName.c_str(),
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      width, height,
                                      (bIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (newWindow)
    {
        if (bIsFullscreen && displayMode != -1)
        {
            SDL_DisplayMode mode;
            SDL_GetDisplayMode(0, displayMode, &mode);
            SDL_SetWindowDisplayMode(newWindow, &mode);
        }

        SDL_GLContext context = SDL_GL_CreateContext(newWindow);
        if (!context)
        {
            printf("Failed to create OpenGL context\n");
        }

        glewExperimental = GL_TRUE;
        glewInit();

        CommonShaders commonShaders;
        commonShaders.build();

        window = newWindow;

        oglVersion = (char *)glGetString(GL_RENDERER); // get renderer string
        version = (char *)glGetString(GL_VERSION);     // version as a string

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

unsigned int View::getTexture()
{
    return renderedTexture;
}

Renderer *View::getRenderer()
{
    return renderer;
}

void View::useFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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
    if (framebuffer)
        glDeleteFramebuffers(1, &framebuffer);
    if (renderedTexture)
        glDeleteTextures(1, &renderedTexture);

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Color texture
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, drawableWidth, drawableHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (renderer)
        delete renderer;
    renderer = new Renderer(drawableWidth, drawableHeight, config);
}