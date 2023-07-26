// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "os/view.h"
#include "stage/layer.h"
#include "math/math.h"
#include "controller/audioController.h"
#include "connector/withAudio.h"
#include "math/transformation.h"
#include <list>

enum class CameraType
{
    None = 0,
    Orto = 1,
    Perspective = 2
};

class Camera : public WithAudio
{
public:
    EXPORT Camera();
    EXPORT virtual void prepareToRender(View *view);
    EXPORT virtual void finishRender();
    EXPORT Matrix4 *getProjectionMatrix();
    EXPORT Matrix4 *getViewMatrix();
    EXPORT void setViewMatrix(Matrix4 &view);
    EXPORT virtual int getWidth();
    EXPORT virtual int getHeight();
    EXPORT virtual float getWidthViewProportion();
    EXPORT virtual float getHeightViewProportion();

    EXPORT Transformation *getOwnerTransform();
    EXPORT void setOwnerTransform(Transformation *transform);

    EXPORT virtual PointWithDirection screenToWorld(float x, float y);

    EXPORT void setAsListenerCamera();
    EXPORT bool isListenerCamera();

    EXPORT virtual CameraType getCameraType();

    EXPORT virtual float getLineThickness();

protected:
    Matrix4 projectionMatrix;
    Matrix4 viewMatrix;
    Transformation *ownerTransform = nullptr;
};
