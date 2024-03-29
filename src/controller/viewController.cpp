// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "controller/viewController.h"
#include <SDL.h>
#include <algorithm>

InputController *ViewController::inputController = nullptr;

ViewController::ViewController(Config *config)
{
    this->config = config;
    logger->logf("Gamepad amount %i\n", SDL_NumJoysticks());
    int amount = SDL_NumJoysticks();
    for (int i = 0; i < amount; i++)
    {
        SDL_Joystick *gGameController = SDL_JoystickOpen(i);
        if (gGameController == NULL)
            logger->logff("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
        else
            gamePads.push_back(GamepadDevice({i, gGameController}));
    }

    cursorDataArrow = SDL_CreateSystemCursor(SDL_SystemCursor::SDL_SYSTEM_CURSOR_ARROW);
    cursorDataHand = SDL_CreateSystemCursor(SDL_SystemCursor::SDL_SYSTEM_CURSOR_HAND);
    cursorDataIBeam = SDL_CreateSystemCursor(SDL_SystemCursor::SDL_SYSTEM_CURSOR_IBEAM);
}

View *ViewController::createView(std::string name, unsigned int flags)
{
    checkConfig();
    View *view = new View(config, flags);

    view->windowName = name;
    if (view->makeWindow())
    {
        logger->logff(
            "New view has been created: %dx%d, %s",
            view->getWidth(),
            view->getHeight(),
            view->isFullscreen() ? "fullscreen" : "windowed");

        views.push_back(view);

        if (!mainView)
            mainView = view;

        DPIZoom = mainView->getDPIZoom();

        return view;
    }

    delete view;
    return nullptr;
}

RenderTarget *ViewController::createRenderTarget(int width, int height, RenderQuality renderQuality)
{
    return new RenderTarget(width, height, renderQuality, 1.0f);
}

void ViewController::destroyRenderTarget(RenderTarget *renderTarget)
{
    if (renderTarget)
        delete renderTarget;
}

void ViewController::update()
{
    if (mainView)
    {
        checkConfig();
        mainView->changeMode();
    }
}

void ViewController::getAvailableResolutions(std::vector<DisplayMode> *modes, bool onlyNative)
{
    SDL_DisplayMode mode;
    modes->clear();
    int displayModes = SDL_GetNumDisplayModes(0);

    for (int i = 0; i < displayModes; i++)
    {
        SDL_GetDisplayMode(0, i, &mode);
        float ratio = (float)mode.w / (float)mode.h;
        float nativeRatio = (float)getPrimaryScreenWidth() / (float)getPrimaryScreenHeight();

        if (ratio == nativeRatio || !onlyNative)
        {
            bool shouldBeAdded = true;
            if (!modes->empty())
                for (auto it = modes->begin(); it != modes->end(); it++)
                {
                    if ((*it).width == mode.w && (*it).height == mode.h)
                    {
                        shouldBeAdded = false;
                        break;
                    }
                }
            if (shouldBeAdded)
                modes->push_back(DisplayMode({mode.w, mode.h, i}));
        }
    }

    if (!modes->empty())
        std::sort(modes->begin(), modes->end(), [](DisplayMode &a, DisplayMode &b)
                  { return a.width * a.height > b.width * b.height; });
}

void ViewController::getAvailableRefreshRates(DisplayMode *mode, std::vector<int> *refreshRates)
{
    SDL_DisplayMode nativeMode;
    int displayModes = SDL_GetNumDisplayModes(0);

    // printf("Refresh Rates: %i\n", displayModes);
    for (int i = 0; i < displayModes; i++)
    {
        SDL_GetDisplayMode(0, i, &nativeMode);
        if (mode->width == nativeMode.w && mode->height == nativeMode.h)
            refreshRates->push_back(nativeMode.refresh_rate);
    }

    if (!refreshRates->empty())
        std::sort(refreshRates->begin(), refreshRates->end(), std::greater<int>());
}

bool ViewController::isResolutionAvailable(int width, int height, int refreshRate)
{
    SDL_DisplayMode mode;
    int displayModes = SDL_GetNumDisplayModes(0);

    for (int i = 0; i < displayModes; i++)
    {
        SDL_GetDisplayMode(0, i, &mode);
        if (mode.w == width && mode.h == height && (refreshRate == 0 || mode.refresh_rate == refreshRate))
            return true;
    }
    return false;
}

bool ViewController::isResolutionAvailable(DisplayMode &mode, int refreshRate)
{
    return isResolutionAvailable(mode.width, mode.height);
}

void ViewController::processEvents()
{
    SDL_Event event;
    for (auto it = views.begin(); it != views.end(); it++)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                isExitIntended = true;
                break;

            case SDL_KEYDOWN:
                // printf("Key press detected %i\n", event.key.keysym.scancode);
                inputController->provideInput(InputType::KEYBOARD, -1, event.key.keysym.scancode, 1.0f);
                break;

            case SDL_TEXTINPUT:
                inputController->provideInputText(event.text.text);
                break;

            case SDL_KEYUP:
                // printf("Key release detected %i\n", event.key.keysym.scancode);
                inputController->provideInput(InputType::KEYBOARD, -1, event.key.keysym.scancode, 0.0f);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button >= 1 && event.button.button <= 5)
                    inputController->provideInput(InputType::MOUSE, -1, event.button.button, 1.0f);
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button >= 1 && event.button.button <= 5)
                    inputController->provideInput(InputType::MOUSE, -1, event.button.button, 0.0f);
                break;

            case SDL_JOYDEVICEADDED:
            {
                SDL_Joystick *gGameController = SDL_JoystickOpen(event.jdevice.which);
                if (gGameController == NULL)
                    logger->logff("Warning: Unable to open game controller!");
                else
                    gamePads.push_back(GamepadDevice({event.jdevice.which, gGameController}));
                break;
            }

            case SDL_JOYDEVICEREMOVED:
            {
                for (auto it = gamePads.begin(); it != gamePads.end(); it++)
                    if (it->index == event.jdevice.which)
                    {
                        SDL_JoystickClose((SDL_Joystick *)it->gamePad);
                        gamePads.erase(it);
                        break;
                    }
            }

            case SDL_JOYAXISMOTION:
            {
                float axisValue = float((int)event.jaxis.value) / float(32 * 1024);
                inputController->provideInput(InputType::GAMEPAD_AXIS, event.jaxis.which, event.jaxis.axis, axisValue);
                break;
            }

            case SDL_JOYBUTTONDOWN:
                inputController->provideInput(InputType::GAMEPAD_BUTTON, event.jbutton.which, event.jbutton.button, 1.0f);
                break;

            case SDL_JOYBUTTONUP:
                inputController->provideInput(InputType::GAMEPAD_BUTTON, event.jbutton.which, event.jbutton.button, 0.0f);
                break;

            case SDL_MOUSEMOTION:
                inputController->provideInput(InputType::MOUSE, (int)InputTypeMouse::MOVEMENT, (int)InputTypeMouseMove::MOVE_HORIZONTAL, event.motion.x);
                inputController->provideInput(InputType::MOUSE, (int)InputTypeMouse::MOVEMENT, (int)InputTypeMouseMove::MOVE_VERTICAL, event.motion.y);
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    for (auto &view : views)
                    {
                        view->updateSize(event.window.data1, event.window.data2);
                    }
                }
                break;

            case SDL_MOUSEWHEEL:
                inputController->provideInput(InputType::MOUSE, (int)InputTypeMouse::MOVEMENT, (int)InputTypeMouseMove::MOVE_WHEEL_VERTICAL, event.wheel.preciseY);
                break;
            }
        }
    }
}

bool ViewController::getIsExitIntended()
{
    return isExitIntended;
}

void ViewController::setInputController(InputController *inputController)
{
    ViewController::inputController = inputController;
}

int ViewController::getPrimaryScreenWidth()
{
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    return DM.w;
}

int ViewController::getPrimaryScreenHeight()
{
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    return DM.h;
}

void ViewController::hideCursor()
{
    bIsCursorShown = false;
    SDL_ShowCursor(SDL_DISABLE);
}

void ViewController::showCursor()
{
    bIsCursorShown = true;
    SDL_ShowCursor(SDL_ENABLE);
}

bool ViewController::isCursorShown()
{
    return bIsCursorShown;
}

void ViewController::setCursorType(CursorType type)
{
    SDL_Cursor *cursor = nullptr;
    if (type == CursorType::Default)
        cursor = reinterpret_cast<SDL_Cursor *>(cursorDataArrow);
    if (type == CursorType::Hand)
        cursor = reinterpret_cast<SDL_Cursor *>(cursorDataHand);
    if (type == CursorType::IBeam)
        cursor = reinterpret_cast<SDL_Cursor *>(cursorDataIBeam);

    if (cursor)
    {
        SDL_SetCursor(cursor);
        cursorType = type;
    }
}

CursorType ViewController::getCursorType()
{
    return cursorType;
}

float ViewController::getDPIZoom()
{
    return DPIZoom;
}

void ViewController::checkConfig()
{
    if (config)
    {
        bool isFullscreen = config->isFullscreen();
        if (config->getWindowWidth() == 0)
            config->setWindowWidth(getPrimaryScreenWidth() / (isFullscreen ? 1 : 2));
        if (config->getWindowHeight() == 0)
            config->setWindowHeight(getPrimaryScreenHeight() / (isFullscreen ? 1 : 2));

        if (isFullscreen)
        {
            if (!isResolutionAvailable(config->getWindowWidth(), config->getWindowHeight(), config->getRefreshRate()))
            {
                logger->logff("Unable to set fullscreen resolution %ix%i, %ihz", config->getWindowWidth(), config->getWindowHeight(), config->getRefreshRate());
                config->setWindowWidth(getPrimaryScreenWidth() / (isFullscreen ? 1 : 2));
                config->setWindowHeight(getPrimaryScreenHeight() / (isFullscreen ? 1 : 2));
                config->setRefreshRate(0);
            }
        }
    }
}