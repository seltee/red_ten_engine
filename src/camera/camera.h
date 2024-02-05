// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/renderTarget.h"
#include "stage/layer.h"
#include "math/math.h"
#include "controller/audioController.h"
#include "connector/withAudio.h"
#include "math/transformation.h"
#include "math/sphere.h"
#include <list>

class Component;

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
    EXPORT virtual void prepareToRender(RenderTarget *renderTarget);
    EXPORT virtual void finishRender();
    EXPORT Matrix4 *getProjectionMatrix();
    EXPORT Matrix4 *getViewMatrix();
    EXPORT void setViewMatrix(Matrix4 &view);
    EXPORT virtual int getWidth();
    EXPORT virtual int getHeight();
    EXPORT virtual float getWidthTargetProportion();
    EXPORT virtual float getHeightTargetProportion();

    EXPORT virtual PointWithDirection screenToWorld(float x, float y);

    EXPORT void setAsListenerCamera();
    EXPORT bool isListenerCamera();

    EXPORT virtual CameraType getCameraType();

    EXPORT virtual float getLineThickness();

    EXPORT Matrix4 getWorldModelMatrix();
    EXPORT Vector3 getDirection();

    inline void setOwner(Component *owner) { this->owner = owner; }

    RenderTarget *renderTarget = nullptr;

    EXPORT virtual void recalcCullingPlanes();
    EXPORT inline Vector4 *getCullingPlanes() { return cullingPlanes; };

protected:
    Vector4 cullingPlanes[6];
    Matrix4 projectionMatrix;
    Matrix4 viewMatrix;
    Component *owner = nullptr;
};
