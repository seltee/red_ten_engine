// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentCameraPerspective.h"
#include "stage/layerActors.h"

ComponentCameraPerspective::~ComponentCameraPerspective()
{
    auto layer = (LayerActors *)owner->getCurrentLayer();
    if (layer->getActiveCamera() == cameraPerspective)
    {
        layer->setActiveCamera(nullptr);
    }
}

void ComponentCameraPerspective::onCreated()
{
    cameraPerspective = new CameraPerspective();
    cameraPerspective->setOwnerTransform(&owner->transform);
    auto layer = (LayerActors *)owner->getCurrentLayer();
    if (!layer->getActiveCamera())
    {
        layer->setActiveCamera(cameraPerspective);
    }
}

bool ComponentCameraPerspective::onRenderPrepare(Matrix4 &vpMatrix, Transformation *tf, bool isShadowStage)
{
    Matrix4 viewMatrix = glm::inverse(*tf->getModelMatrix() * *transform.getModelMatrix());
    cameraPerspective->setViewMatrix(viewMatrix);
    if (cameraPerspective->isListenerCamera())
    {
        audio->getAudioBase()->setPosition(transform.getPosition());
        audio->getAudioBase()->setOrientation(transform.getRotation());
    }
    return true;
}

CameraPerspective *ComponentCameraPerspective::getCamera()
{
    return cameraPerspective;
}