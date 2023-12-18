#pragma once
#include "common/utils.h"
#include "math/math.h"
#include "os/view.h"
#include "connector/withAudio.h"
#include "camera/cameraPerspective.h"
#include "component/component.h"

class ComponentCameraPerspective : public Component, public WithAudio
{
public:
    EXPORT ~ComponentCameraPerspective();
    EXPORT void onCreated();
    EXPORT CameraPerspective *getCamera();

protected:
    CameraPerspective *cameraPerspective = nullptr;
};