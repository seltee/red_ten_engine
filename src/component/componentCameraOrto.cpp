// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentCameraOrto.h"
#include "stage/layerActors.h"

ComponentCameraOrto::~ComponentCameraOrto()
{
    auto layer = (LayerActors *)owner->getCurrentLayer();
    if (layer->getActiveCamera() == cameraOrto)
    {
        layer->setActiveCamera(nullptr);
    }
}

void ComponentCameraOrto::onCreated()
{
    cameraOrto = new CameraOrto();
    cameraOrto->setOwnerTransform(&owner->transform);
    auto layer = (LayerActors *)owner->getCurrentLayer();
    if (!layer->getActiveCamera())
    {
        layer->setActiveCamera(cameraOrto);
    }
}

bool ComponentCameraOrto::onRenderPrepare(Matrix4 &vpMatrix, Transformation *tf, bool isShadowStage)
{
    Matrix4 viewMatrix = glm::inverse(*tf->getModelMatrix() * *transform.getModelMatrix());
    cameraOrto->setViewMatrix(viewMatrix);
    if (cameraOrto->isListenerCamera())
    {
        audio->getAudioBase()->setPosition(transform.getPosition());
        audio->getAudioBase()->setOrientation(transform.getRotation());
    }
    return true;
}

CameraOrto *ComponentCameraOrto::getCamera()
{
    return cameraOrto;
}