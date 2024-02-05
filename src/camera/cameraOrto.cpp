// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "camera/cameraOrto.h"
#include "math/glm/gtc/type_ptr.hpp"

void CameraOrto::prepareToRender(RenderTarget *renderTarget)
{
    float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
    float targetWidth = useWidthBasedProportion ? mainLine : mainLine * aspect;
    float targetHeight = useWidthBasedProportion ? mainLine / aspect : mainLine;

    right = targetWidth / 2.0f;
    left = -right;
    bottom = -targetHeight / 2.0f;
    top = -bottom;
    far = 1400.0f;
    near = -1400.0f;

    projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
    this->renderTarget = renderTarget;

    viewMatrix = glm::inverse(getWorldModelMatrix());
    recalcCullingPlanes();
}

void CameraOrto::finishRender()
{
}

int CameraOrto::getWidth()
{
    if (renderTarget && renderTarget->getHeight() > 0 && renderTarget->getWidth() > 0)
    {
        float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
        return (int)(useWidthBasedProportion ? mainLine : mainLine * aspect);
    }
    return 0;
}

int CameraOrto::getHeight()
{
    if (renderTarget && renderTarget->getHeight() && renderTarget->getWidth() > 0)
    {
        float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
        return (int)(useWidthBasedProportion ? mainLine / aspect : mainLine);
    }
    return 0;
}

float CameraOrto::getWidthTargetProportion()
{
    if (renderTarget)
    {
        float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
        float targetWidth = useWidthBasedProportion ? mainLine : mainLine * aspect;
        return targetWidth / (float)renderTarget->getWidth();
    }
    return 0;
}

float CameraOrto::getHeightTargetProportion()
{
    float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
    float targetHeight = useWidthBasedProportion ? mainLine / aspect : mainLine;
    return targetHeight / (float)renderTarget->getHeight();
}

void CameraOrto::setWidthBasedResolution(float width)
{
    useWidthBasedProportion = true;
    mainLine = width;
}

void CameraOrto::setHeightBasedResolution(float height)
{
    useWidthBasedProportion = false;
    mainLine = height;
}

void CameraOrto::recalcCullingPlanes()
{
    // Near and far planes
    cullingPlanes[0] = Vector4(0, 0, -1, -near); // Near
    cullingPlanes[1] = Vector4(0, 0, 1, far);    // Far

    // Left and right planes
    cullingPlanes[2] = Vector4(1, 0, 0, -left); // Left
    cullingPlanes[3] = Vector4(-1, 0, 0, right); // Right

    // Bottom and top planes
    cullingPlanes[4] = Vector4(0, 1, 0, top);     // Top
    cullingPlanes[5] = Vector4(0, -1, 0, -bottom); // Bottom
}

PointWithDirection CameraOrto::screenToWorld(float x, float y)
{
    PointWithDirection out;
    if (!renderTarget)
    {
        out.vDirection = Vector3(0.0f, 0.0f, 1.0f);
        out.vPosition = Vector3(0.0f, 0.0f, 0.0f);
        return out;
    }

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

float CameraOrto::getLineThickness()
{
    return getWidth() / 1000.0f + 0.0002f;
}

CameraType CameraOrto::getCameraType()
{
    return CameraType::Orto;
}