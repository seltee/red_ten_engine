// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "camera/cameraOrto.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "opengl/glew.h"

void CameraOrto::prepareToRender(RenderTarget *renderTarget)
{
    float aspect = (float)renderTarget->getWidth() / (float)renderTarget->getHeight();
    float targetWidth = useWidthBasedProportion ? mainLine : mainLine * aspect;
    float targetHeight = useWidthBasedProportion ? mainLine / aspect : mainLine;

    float r = targetWidth / 2.0f;
    float l = -r;
    float b = -targetHeight / 2.0f;
    float t = -b;
    float f = 1400.0f;
    float n = -1400.0f;

    projectionMatrix = glm::ortho(l, r, b, t, n, f);
    this->renderTarget = renderTarget;
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

float CameraOrto::getLineThickness()
{
    return 1.2f;
}

CameraType CameraOrto::getCameraType()
{
    return CameraType::Orto;
}