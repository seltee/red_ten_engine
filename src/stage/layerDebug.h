// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "shaders/effect.h"
#include "stage/layerActors.h"
#include "stage/layer.h"
#include "resource/resourceFont.h"
#include "component/componentText.h"
#include "common/color.h"
#include "controller/debugController.h"
#include "actor/actorCamera.h"
#include <vector>

class LayerDebug : public LayerActors
{
public:
    LayerDebug(int index);
    void process(float delta);

    static void setFont(ResourceFont *font);
    static void setDebugController(DebugController *debugController);
protected:
    bool hasIndex(int index);

    ActorCamera *cameraActor;
    CameraOrto *camera;
    std::vector<Actor*> debugActors;

    static ResourceFont *font;
    static DebugController *debugController;
};
