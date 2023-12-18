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
    cameraPerspective->setOwner(this);
    auto layer = (LayerActors *)owner->getCurrentLayer();
    if (!layer->getActiveCamera())
    {
        layer->setActiveCamera(cameraPerspective);
    }
}

CameraPerspective *ComponentCameraPerspective::getCamera()
{
    return cameraPerspective;
}