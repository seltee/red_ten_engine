// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "core/core.h"
#include "common/utils.h"
#include "common/soundPlayer.h"
#include "common/pawn.h"
#include "common/keyboard.h"
#include "common/meshMaker.h"
#include "connector/withLogger.h"
#include "connector/withDebug.h"
#include "connector/withRepository.h"
#include "connector/withMeshMaker.h"
#include "connector/withAudio.h"
#include "connector/withProfiler.h"
#include "resource/resourceMesh.h"
#include "resource/resourceFont.h"
#include "resource/resourceSound.h"
#include "resource/resourceImage.h"
#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "mesh/meshCompound.h"
#include "actor/actor.h"
#include "actor/actorPawn.h"
#include "actor/actorGUIElement.h"
#include "actor/actorCamera.h"
#include "actor/watchable.h"
#include "actor/watcher.h"
#include "physics/shapes/shape.h"
#include "physics/shapes/shapeBox.h"
#include "physics/shapes/shapeGeometry.h"
#include "physics/shapes/shapePlain.h"
#include "physics/shapes/shapeSphere.h"
#include "camera/camera.h"
#include "camera/cameraOrto.h"
#include "camera/cameraPerspective.h"
#include "controller/viewController.h"
#include "controller/stageController.h"
#include "controller/resourceController.h"
#include "controller/inputController.h"
#include "controller/audioController.h"
#include "controller/logController.h"
#include "controller/configController.h"
#include "controller/debugController.h"
#include "controller/profilerController.h"
#include "component/component.h"
#include "component/componentSprite.h"
#include "component/componentFramedSprite.h"
#include "component/componentSoundPlayer.h"
#include "component/componentText.h"
#include "component/componentMesh.h"
#include "component/componentAnimatedMesh.h"
#include "component/componentLight.h"
#include "component/componentCameraOrto.h"
#include "component/componentCameraPerspective.h"
#include "renderer/texture.h"
#include "renderer/renderer.h"
#include "stage/stage.h"
#include "math/math.h"
#include "renderer/shader.h"

#ifdef _WIN32
#include <shellscalingapi.h>
#include <windows.h>
#endif

#ifdef _WIN32
#define APPMAIN int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define APPMAIN int main(int argc, char *argv[])
#endif

class RTEngine
{
protected:
    RTEngine(std::string configFilePath, bool showConsole);

    static ViewController *viewController;
    static StageController *stageController;
    static ResourceController *resourceController;
    static InputController *inputController;
    static AudioController *audioController;
    static LogController *logController;
    static ConfigController *configController;
    static DebugController *debugController;
    static ProfilerController *profilerController;

    static MeshMaker *meshMaker;

    static bool isSDLInitDone;

    Stage *currentStage = nullptr;

    unsigned int tick = 0;
    bool bTerminationRequested = false;

    static RTEngine *instance;

    Core *core = nullptr;

public:
    EXPORT static RTEngine *getInstance(std::string configFilePath = "./cfg", bool showConsole = true);

    EXPORT static ViewController *getViewController();
    EXPORT static StageController *getStageController();
    EXPORT static ResourceController *getResourceController();
    EXPORT static InputController *getInputController();
    EXPORT static AudioController *getAudioController();
    EXPORT static LogController *getLogController();
    EXPORT static ConfigController *getConfigController();
    EXPORT static DebugController *getDebugController();
    EXPORT static ProfilerController *getProfilerController();
    EXPORT static MeshMaker *getMeshMaker();

    EXPORT inline Core *getCore() { return core; };

    EXPORT void openUrl(const char *url);

    EXPORT float syncFrame();
    EXPORT void terminate();
    EXPORT bool isTerminationIntended();

    EXPORT void destroy();
};
