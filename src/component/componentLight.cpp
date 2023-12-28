// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentLight.h"
#include "common/utils.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "camera/camera.h"

ComponentLight::ComponentLight()
{
}

void ComponentLight::setupSunLight(Vector3 color, bool bCastShadows, float affectDistance)
{
    type = LightType::Sun;
    this->color = color;
    this->affectDistance = affectDistance;
    this->bCastShadows = bCastShadows;
}

void ComponentLight::setupOmniLight(float affectDistance, Vector3 color, bool bCastShadows)
{
    type = LightType::Omni;
    this->affectDistance = affectDistance;
    this->color = color;
    this->bCastShadows = bCastShadows;
}

void ComponentLight::onRenderQueue(RenderQueue *renderQueue)
{
    Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix();
    Vector4 v4Position = mModel * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    renderQueue->addLight(type, Vector3(v4Position), color, affectDistance, bCastShadows);
}

void ComponentLight::enableShadows()
{
    bCastShadows = true;
}

void ComponentLight::disableShadows()
{
    bCastShadows = false;
}
