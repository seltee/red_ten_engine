#pragma once
#include "common/utils.h"
#include "math/math.h"
#include "os/view.h"
#include "connector/withAudio.h"
#include "camera/cameraOrto.h"
#include "component/component.h"

class ComponentCameraOrto : public Component, public WithAudio
{
public:
    EXPORT ~ComponentCameraOrto();

    EXPORT void onCreated();
    EXPORT bool onRenderPrepare(Matrix4 &vpMatrix, Transformation *tf, bool isShadowStage);

    EXPORT CameraOrto *getCamera();

protected:
    CameraOrto *cameraOrto = nullptr;
};