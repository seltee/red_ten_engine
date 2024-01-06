// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "rtengine.h"
#include "stage/layerDebug.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <cmath>
#include <cstdio>
#include <fcntl.h>
#include <io.h>

RTEngine *RTEngine::instance = nullptr;

ViewController *RTEngine::viewController = nullptr;
StageController *RTEngine::stageController = nullptr;
ResourceController *RTEngine::resourceController = nullptr;
InputController *RTEngine::inputController = nullptr;
AudioController *RTEngine::audioController = nullptr;
LogController *RTEngine::logController = nullptr;
ConfigController *RTEngine::configController = nullptr;
DebugController *RTEngine::debugController = nullptr;
ProfilerController *RTEngine::profilerController = nullptr;

MeshMaker *RTEngine::meshMaker = nullptr;

bool RTEngine::isSDLInitDone = false;

RTEngine::RTEngine(std::string configFilePath, bool showConsole)
{
    if (showConsole)
    {
        AllocConsole();
        freopen("conin$", "r", stdin);
        freopen("conout$", "w", stdout);
        freopen("conout$", "w", stderr);
    }

    Config *config = nullptr;

    core = new Core();
    WithCore::setGlobalCore(core);

    if (!logController)
    {
        logController = new LogController("log.txt");
        WithLogger::setLogController(logController);
    }
    if (!profilerController)
    {
        profilerController = new ProfilerController(logController);
        WithProfiler::setProfilerController(profilerController);
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
}

RTEngine *RTEngine::getInstance(std::string configFilePath, bool showConsole)
{
    if (!isSDLInitDone)
    {
        isSDLInitDone = true;

#ifdef _WIN32
        SetProcessDPIAware();
#endif

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
            return nullptr;
        TTF_Init();
    }

    if (!instance)
        instance = new RTEngine(configFilePath, showConsole);
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

AudioController *RTEngine::getAudioController()
{
    return audioController;
}

LogController *RTEngine::getLogController()
{
    return logController;
}

ConfigController *RTEngine::getConfigController()
{
    return configController;
}

DebugController *RTEngine::getDebugController()
{
    return debugController;
}

ProfilerController *RTEngine::getProfilerController()
{
    return profilerController;
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

    audioController->process(delta);
    debugController->process(delta);

    profilerController->frameSync();
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

void RTEngine::destroy()
{
    instance = nullptr;

    delete viewController;
    viewController = nullptr;
    delete stageController;
    stageController = nullptr;
    delete resourceController;
    resourceController = nullptr;
    delete inputController;
    inputController = nullptr;
    delete audioController;
    audioController = nullptr;
    delete logController;
    logController = nullptr;
    delete configController;
    configController = nullptr;
    delete debugController;
    debugController = nullptr;
    delete profilerController;
    profilerController = nullptr;

    delete this;
}