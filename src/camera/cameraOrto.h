// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/renderTarget.h"
#include "camera/camera.h"
#include "stage/layer.h"

class CameraOrto : public Camera
{
public:
    EXPORT void prepareToRender(RenderTarget *renderTarget) override;
    EXPORT void finishRender() override;
    EXPORT int getWidth() override;
    EXPORT int getHeight() override;
    EXPORT float getWidthTargetProportion() override;
    EXPORT float getHeightTargetProportion() override;

    EXPORT void setWidthBasedResolution(float width);
    EXPORT void setHeightBasedResolution(float height);

    EXPORT PointWithDirection screenToWorld(float x, float y) override;

    EXPORT float getLineThickness() override;

    EXPORT CameraType getCameraType() override;

protected:
    Layer *layer;
    bool useWidthBasedProportion = true;
    float mainLine = 320.0f;
};