#pragma once
#include "actor/actor.h"
#include "camera/camera.h"
#include "camera/cameraOrto.h"
#include "camera/cameraPerspective.h"
#include "component/componentCameraOrto.h"
#include "component/componentCameraPerspective.h"

class ActorCamera : public Actor
{
public:
    CameraOrto *setupOrtoCamera();
    CameraPerspective *setupPerspectiveCamera();

    inline Camera *getCamera() { return camera; };
    inline Component *getCameraComponent() { return componentCamera; }

protected:
    Component *componentCamera = nullptr;
    Camera *camera = nullptr;
};