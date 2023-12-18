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
    cameraOrto->setOwner(this);
    auto layer = (LayerActors *)owner->getCurrentLayer();
    if (!layer->getActiveCamera())
    {
        layer->setActiveCamera(cameraOrto);
    }
}

CameraOrto *ComponentCameraOrto::getCamera()
{
    return cameraOrto;
}