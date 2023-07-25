// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "camera/camera.h"
#include "math/math.h"
#include "common/utils.h"
#include "math/glm/gtc/type_ptr.hpp"

AudioController *Camera::audioController = nullptr;

Camera::Camera()
{
    registerClassName("Camera");
    if (!audioController->getListenerCamera())
    {
        setAsListenerCamera();
    }
}

void Camera::prepareToRender(View *view)
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
    viewMatrix = glm::inverse(*transform.getModelMatrix());
    return &viewMatrix;
}

int Camera::getWidth()
{
    return 0;
}

int Camera::getHeight()
{
    return 0;
}

float Camera::getWidthViewProportion()
{
    return 0;
}

float Camera::getHeightViewProportion()
{
    return 0;
}

PointWithDirection Camera::screenToWorld(float x, float y)
{
    PointWithDirection s;
    s.vPosition = Vector3({0.0f, 0.0f, 0.0f});
    s.vDirection = Vector3({0.0f, 0.0f, 0.0f});
    return s;
}

void Camera::onProcess(float delta)
{
    if (isListenerCamera())
    {
        audioController->getAudioBase()->setPosition(transform.getPosition());
        audioController->getAudioBase()->setOrientation(transform.getRotation());
    }
}

void Camera::setAsListenerCamera()
{
    audioController->setListenerCamera(this);
}

bool Camera::isListenerCamera()
{
    return this == audioController->getListenerCamera();
}

float Camera::getLineThickness()
{
    return 0.01f;
}

void Camera::setAudioController(AudioController *audioController)
{
    Camera::audioController = audioController;
}