// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/texture.h"
#include "component/component.h"
#include "renderer/shader.h"
#include "connector/withRenderer.h"

class ComponentSprite : public Component, public WithRenderer
{
public:
    EXPORT ComponentSprite();

    EXPORT void onRenderQueue(RenderQueue *renderQueue) override final;

    EXPORT void setOpacity(float opacity);
    EXPORT float getOpacity();
    EXPORT void setAnchor(float x, float y);
    EXPORT void setAnchor(Vector2 &anchor);
    EXPORT void setRelativeScale(float x, float y);
    EXPORT void setRelativeScale(Vector2 &size);
    EXPORT void setTexture(Texture *texture);
    EXPORT Texture *getTexture();
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix() override;

protected:
    float opacity = 1.0f;
    Texture *texture = nullptr;
    Shader *shader = nullptr;
    MeshStatic *mesh = nullptr;
    Matrix4 mAnchor;
};
