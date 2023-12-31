// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "camera/cameraPerspective.h"
#include "math/glm/gtc/type_ptr.hpp"

void CameraPerspective::prepareToRender(RenderTarget *renderTarget)
{
    float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
    float targetWidth = useWidthBasedProportion ? mainLine : mainLine * aspect;
    float targetHeight = useWidthBasedProportion ? mainLine / aspect : mainLine;

    projectionMatrix = glm::perspective(distance, (float)targetWidth / (float)targetHeight, nearDistance, farDistance);
    this->renderTarget = renderTarget;

    viewMatrix = glm::inverse(getWorldModelMatrix());
}

void CameraPerspective::finishRender()
{
}

int CameraPerspective::getWidth()
{
    if (renderTarget && renderTarget->getHeight() > 0 && renderTarget->getWidth() > 0)
    {
        float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
        return (int)(useWidthBasedProportion ? mainLine : mainLine * aspect);
    }
    return 0;
}

int CameraPerspective::getHeight()
{
    if (renderTarget && renderTarget->getHeight() > 0 && renderTarget->getWidth() > 0)
    {
        float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
        return (int)(useWidthBasedProportion ? mainLine / aspect : mainLine);
    }
    return 0;
}

float CameraPerspective::getWidthTargetProportion()
{
    float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
    float targetWidth = useWidthBasedProportion ? mainLine : mainLine * aspect;
    return targetWidth / (float)renderTarget->getWidth();
}

float CameraPerspective::getHeightTargetProportion()
{
    float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
    float targetHeight = useWidthBasedProportion ? mainLine / aspect : mainLine;
    return targetHeight / (float)renderTarget->getHeight();
}

void CameraPerspective::setWidthBasedResolution(float width)
{
    useWidthBasedProportion = true;
    mainLine = width;
}

void CameraPerspective::setHeightBasedResolution(float height)
{
    useWidthBasedProportion = false;
    mainLine = height;
}

void CameraPerspective::setFarDistance(float farDistance)
{
    this->farDistance = farDistance;
}

void CameraPerspective::setNearDistance(float nearDistance)
{
    this->nearDistance = nearDistance;
}

void CameraPerspective::setFov(float fov)
{
    this->fov = fov;
}

PointWithDirection CameraPerspective::screenToWorld(float x, float y)
{
    PointWithDirection out;
    if (!renderTarget)
        return out;

    float halfWidth = renderTarget->getWidth() / 2.0f;
    float halfHeight = renderTarget->getHeight() / 2.0f;

    Matrix4 mView = glm::inverse(projectionMatrix * *getViewMatrix());

    Vector4 near = Vector4((x - halfWidth) / halfWidth, -1 * (y - halfHeight) / halfHeight, 0.0f, 1.0);
    Vector4 far = Vector4((x - halfWidth) / halfWidth, -1 * (y - halfHeight) / halfHeight, 1.0f, 1.0);

    Vector4 nearResult = mView * near;
    Vector4 farResult = mView * far;
    nearResult /= nearResult.w;
    farResult /= farResult.w;

    out.vDirection = glm::normalize(Vector3(farResult - nearResult));

    out.vPosition = Vector3({nearResult.x,
                             nearResult.y,
                             nearResult.z});

    return out;
}

float CameraPerspective::getLineThickness()
{
    return 0.02f;
}

CameraType CameraPerspective::getCameraType()
{
    return CameraType::Perspective;
}