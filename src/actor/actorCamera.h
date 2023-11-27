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
    EXPORT CameraOrto *setupOrtoCamera();
    EXPORT CameraPerspective *setupPerspectiveCamera();

    inline Camera *getCamera() { return camera; };
    inline Component *getCameraComponent() { return componentCamera; }

    EXPORT bool setActive();

protected:
    Component *componentCamera = nullptr;
    Camera *camera = nullptr;
};