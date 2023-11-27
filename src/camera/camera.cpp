// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "camera/camera.h"
#include "math/math.h"
#include "common/utils.h"
#include "math/glm/gtc/type_ptr.hpp"

Camera::Camera()
{
    if (!audio->getListenerCamera())
        setAsListenerCamera();
}

void Camera::prepareToRender(RenderTarget *renderTarget)
{
}

void Camera::finishRender()
{
}

Matrix4 *Camera::getProjectionMatrix()
{
    return &projectionMatrix;
}

Matrix4 *Camera::getViewMatrix()
{
    return &viewMatrix;
}

void Camera::setViewMatrix(Matrix4 &view)
{
    this->viewMatrix = view;
}

int Camera::getWidth()
{
    return 0;
}

int Camera::getHeight()
{
    return 0;
}

float Camera::getWidthTargetProportion()
{
    return 0;
}

float Camera::getHeightTargetProportion()
{
    return 0;
}

Transformation *Camera::getOwnerTransform()
{
    return ownerTransform;
}

void Camera::setOwnerTransform(Transformation *transform)
{
    ownerTransform = transform;
}

PointWithDirection Camera::screenToWorld(float x, float y)
{
    PointWithDirection s;
    s.vPosition = Vector3({0.0f, 0.0f, 0.0f});
    s.vDirection = Vector3({0.0f, 0.0f, 0.0f});
    return s;
}

void Camera::setAsListenerCamera()
{
    audio->setListenerCamera(this);
}

bool Camera::isListenerCamera()
{
    return this == audio->getListenerCamera();
}

CameraType Camera::getCameraType()
{
    return CameraType::None;
}

float Camera::getLineThickness()
{
    return 0.01f;
}
