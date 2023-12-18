// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "rtengine.h"
#include "common/utils.h"
#include "renderer/texture.h"
#include "component/component.h"
#include "renderer/shader.h"
#include "renderer/shaderParameter.h"
#include "connector/withRenderer.h"

class ComponentFramedSprite : public Component, public WithRenderer
{
public:
    EXPORT ComponentFramedSprite();
    EXPORT ~ComponentFramedSprite();

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

    EXPORT void setFramedRender(int framesInLine, int frameRows);
    EXPORT void setFrame(int frame);

    EXPORT inline int getFramesTotalCount() { return framesTotal; };
    EXPORT inline int getFramesInLineCount() { return framesInLine; };

    EXPORT Matrix4 getLocalspaceMatrix() override;

protected:
    void calcFrameRenderSize();

    int framesInLine = 1;
    int framesTotal = 1;
    int currentFrame = 0;
    float frameShift[2] = {0.0f, 0.0f};
    float frameSize[2] = {1.0f, 1.0f};
    float frameRenderSize[2] = {1.0f, 1.0f};

    float opacity = 1.0f;
    Texture *texture = nullptr;
    Shader *shader = nullptr;
    MeshStatic *mesh = nullptr;
    Matrix4 mAnchor;

    ShaderParameter *shadersParameters[2];
};
