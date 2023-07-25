// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/soundPlayer.h"
#include "common/mesh.h"
#include "common/pawn.h"
#include "common/keyboard.h"
#include "common/meshMaker.h"
#include "connector/withLogger.h"
#include "connector/withDebug.h"
#include "connector/withRepository.h"
#include "connector/withMeshMaker.h"
#include "resource/resourceMesh.h"
#include "resource/resourceFont.h"
#include "resource/sound.h"
#include "resource/texture.h"
#include "actor/actor.h"
#include "actor/actorPawn.h"
#include "actor/actorGUIElement.h"
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
#include "component/component.h"
#include "component/componentSprite.h"
#include "component/componentFramedSprite.h"
#include "component/componentSoundPlayer.h"
#include "component/componentText.h"
#include "component/componentMesh.h"
#include "component/componentLight.h"
#include "stage/stage.h"
#include "math/math.h"
#include "actor/watchable.h"
#include "actor/watcher.h"
#include "shaders/phongShader.h"
#include "shaders/rawShader.h"
#include "shaders/effect.h"

class RTEngine
{
protected:
    RTEngine(std::string configFilePath);

    static ViewController *viewController;
    static StageController *stageController;
    static ResourceController *resourceController;
    static InputController *inputController;
    static AudioController *audioController;
    static LogController *logController;
    static ConfigController *configController;
    static DebugController *debugController;

    static MeshMaker *meshMaker;

    static bool isSDLInitDone;

    Stage *currentStage = nullptr;

    unsigned int tick = 0;
    int fps = 0, fpsLastCheckTick = 0, fpsCounter = 0;
    bool bTerminationRequested = false;

public:
    EXPORT static RTEngine *createInstance(std::string configFilePath = "./cfg");

    EXPORT static ViewController *getViewController();
    EXPORT static StageController *getStageController();
    EXPORT static ResourceController *getResourceController();
    EXPORT static InputController *getInputController();
    EXPORT static LogController *getLogController();
    EXPORT static ConfigController *getConfigController();
    EXPORT static DebugController *getDebugController();

    EXPORT static MeshMaker *getMeshMaker();

    EXPORT void openUrl(const char *url);

    EXPORT float syncFrame();
    EXPORT void terminate();
    EXPORT bool isTerminationIntended();
};
