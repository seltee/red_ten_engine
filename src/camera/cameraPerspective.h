// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/renderTarget.h"
#include "camera/camera.h"
#include "stage/layer.h"

class CameraPerspective : public Camera
{
public:
    EXPORT void prepareToRender(RenderTarget *renderTarget);
    EXPORT void finishRender();
    EXPORT int getWidth();
    EXPORT int getHeight();
    EXPORT float getWidthTargetProportion();
    EXPORT float getHeightTargetProportion();

    EXPORT void setWidthBasedResolution(float width);
    EXPORT void setHeightBasedResolution(float height);

    EXPORT void setFarDistance(float farDistance);
    EXPORT void setNearDistance(float nearDistance);
    EXPORT void setFov(float fov);

    EXPORT PointWithDirection screenToWorld(float x, float y);

    EXPORT float getLineThickness();

    EXPORT CameraType getCameraType();

    RenderTarget *renderTarget = nullptr;

protected:
    Layer *layer;
    bool useWidthBasedProportion = true;
    float mainLine = 320.0f;
    float farDistance = 400.0f;
    float nearDistance = 0.1f;
    float fov = 45.0f;
    float distance = 45.0f;
};