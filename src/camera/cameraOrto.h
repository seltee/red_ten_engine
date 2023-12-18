// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/renderTarget.h"
#include "camera/camera.h"
#include "stage/layer.h"

class CameraOrto : public Camera
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
    
    EXPORT float getLineThickness();

    EXPORT CameraType getCameraType();

protected:
    Layer *layer;
    bool useWidthBasedProportion = true;
    float mainLine = 320.0f;
};