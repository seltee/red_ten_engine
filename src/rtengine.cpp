// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "rtengine.h"
#include "common/commonShaders.h"
#include "stage/layerDebug.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <cmath>

#ifdef _WIN32
#include <shellscalingapi.h>
#include <windows.h>
#endif

ViewController *RTEngine::viewController = nullptr;
StageController *RTEngine::stageController = nullptr;
ResourceController *RTEngine::resourceController = nullptr;
InputController *RTEngine::inputController = nullptr;
AudioController *RTEngine::audioController = nullptr;
LogController *RTEngine::logController = nullptr;
ConfigController *RTEngine::configController = nullptr;
DebugController *RTEngine::debugController = nullptr;

MeshMaker *RTEngine::meshMaker = nullptr;

bool RTEngine::isSDLInitDone = false;

RTEngine::RTEngine(std::string configFilePath)
{
    Config *config = nullptr;
    if (!logController)
    {
        logController = new LogController("log.txt");
        WithLogger::setLogController(logController);
    }
    if (!debugController)
    {
        debugController = new DebugController();
        LayerDebug::setDebugController(debugController);
        WithDebug::setDebugController(debugController);
    }
    if (!configController)
    {
        configController = new ConfigController();
        config = configController->getConfig();
        config->setupByQuality(RenderQuality::Balanced);
        config->setConfigFilePath(configFilePath);
        if (config->loadConfig())
            logController->logff("Configuration loaded\n");
        else
            logController->logff("No configuration found\n");
    }
    if (!inputController)
    {
        inputController = new InputController();
        ViewController::setInputController(inputController);
    }

    if (!viewController)
    {
        viewController = new ViewController(config);
        configController->setViewController(viewController);
    }

    if (!stageController)
    {
        stageController = new StageController();
    }

    if (!resourceController)
    {
        resourceController = new ResourceController();
        LayerDebug::setFont(resourceController->addFont(28));
        WithRepository::setResourceController(resourceController);
    }

    if (!audioController)
    {
        audioController = new AudioController(config);
        configController->setAudioController(audioController);
        WithAudio::setAudioController(audioController);
    }

    if (!meshMaker)
    {
        meshMaker = new MeshMaker();
        WithMeshMaker::setMeshMaker(meshMaker);
    }

    tick = SDL_GetTicks();
    fpsLastCheckTick = tick;
}

RTEngine *RTEngine::createInstance(std::string configFilePath)
{
    if (!isSDLInitDone)
    {

#ifdef _WIN32
        SetProcessDPIAware();
#endif

        isSDLInitDone = true;
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
            return nullptr;
        TTF_Init();
    }

    auto instance = new RTEngine(configFilePath);
    return instance;
}

ViewController *RTEngine::getViewController()
{
    return viewController;
}

StageController *RTEngine::getStageController()
{
    return stageController;
}

ResourceController *RTEngine::getResourceController()
{
    return resourceController;
}

InputController *RTEngine::getInputController()
{
    return inputController;
}

ConfigController *RTEngine::getConfigController()
{
    return configController;
}

DebugController *RTEngine::getDebugController()
{
    return debugController;
}

MeshMaker *RTEngine::getMeshMaker()
{
    return meshMaker;
}

void RTEngine::openUrl(const char *url)
{
    SDL_OpenURL(url);
}

float RTEngine::syncFrame()
{
    unsigned int newTick = SDL_GetTicks();
    float delta = fmin((float)(newTick - tick) / 1000.0f, 0.1f);
    tick = newTick;
    fpsCounter++;

    if (newTick - fpsLastCheckTick > 1000)
    {
        fpsLastCheckTick = newTick;
        fps = fpsCounter;
        fpsCounter = 0;
        logController->logConsole("FPS %i", fps);
    }

    audioController->process(delta);
    debugController->process(delta);
    return delta;
}

void RTEngine::terminate()
{
    bTerminationRequested = true;
}

bool RTEngine::isTerminationIntended()
{
    return viewController->getIsExitIntended() | bTerminationRequested;
}
