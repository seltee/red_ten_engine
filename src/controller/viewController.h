// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/config.h"
#include "renderer/renderTarget.h"
#include "os/view.h"
#include "connector/withLogger.h"
#include "controller/inputController.h"
#include <list>

enum class CursorType
{
    Default,
    Hand,
    IBeam
};

struct DisplayMode
{
    int width;
    int height;
    int index;
};

struct GamepadDevice
{
    int index;
    void *gamePad;
};

class ViewController : public WithLogger
{
public:
    ViewController(Config *config);

    EXPORT View *createView(std::string name, unsigned int flags = VIEW_CREATION_FLAG_NONE);
    EXPORT RenderTarget *createRenderTarget(int width, int height, RenderQuality renderQuality);
    EXPORT void destroyRenderTarget(RenderTarget *renderTarget);
    EXPORT void update();

    EXPORT void getAvailableResolutions(std::vector<DisplayMode> *modes, bool onlyNative = true);
    EXPORT void getAvailableRefreshRates(DisplayMode *mode, std::vector<int> *refreshRates);

    EXPORT bool isResolutionAvailable(int width, int height, int refreshRate = 0);
    EXPORT bool isResolutionAvailable(DisplayMode &mode, int refreshRate);

    EXPORT void processEvents();
    EXPORT bool getIsExitIntended();

    EXPORT static void setInputController(InputController *inputController);

    EXPORT int getPrimaryScreenWidth();
    EXPORT int getPrimaryScreenHeight();

    EXPORT void hideCursor();
    EXPORT void showCursor();
    EXPORT bool isCursorShown();
    EXPORT void setCursorType(CursorType type);
    EXPORT CursorType getCursorType();

    EXPORT float getDPIZoom();

protected:
    void checkConfig();

    std::list<View *> views;
    View *mainView = nullptr;
    std::vector<GamepadDevice> gamePads;
    bool isExitIntended = false;
    bool bIsCursorShown = true;
    Config *config = nullptr;
    static InputController *inputController;

    float DPIZoom = 1.0f;

    CursorType cursorType = CursorType::Default;
    void *cursorDataArrow = nullptr;
    void *cursorDataHand = nullptr;
    void *cursorDataIBeam = nullptr;
};
