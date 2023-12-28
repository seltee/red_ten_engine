// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "component/component.h"
#include "renderer/shader.h"
#include "resource/resourceMesh.h"


class ComponentLight : public Component
{
public:
    EXPORT ComponentLight();
    EXPORT void setupSunLight(Vector3 color, bool bCastShadows = false, float affectDistance = 36.0f);
    EXPORT void setupOmniLight(float affectDistance, Vector3 color, bool bCastShadows = false);

    EXPORT void onRenderQueue(RenderQueue *renderQueue) override;

    EXPORT void enableShadows();
    EXPORT void disableShadows();

protected:
    Vector3 color;
    float affectDistance = 0.0f;
    LightType type = LightType::None;
    Matrix4 m1;
    Matrix4 lightViewProjection;

    Transformation *tf = nullptr;
    bool bCastShadows = false;
};
