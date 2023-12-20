// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "common/utils.h"
#include "renderer/texture.h"
#include "connector/withLogger.h"
#include "connector/withRenderer.h"
#include "component/component.h"
#include "resource/resourceFont.h"

class ComponentText : public Component, public WithLogger, public WithRenderer
{
public:
    EXPORT ComponentText();
    EXPORT ~ComponentText();

    EXPORT void process(float delta) override final;
    EXPORT void onRenderQueue(RenderQueue *renderQueue) override final;

    EXPORT void setOpacity(float opacity);
    EXPORT float getOpacity();
    EXPORT void setAnchor(float x, float y);
    EXPORT void setAnchor(Vector2 &anchor);

    EXPORT void setFont(ResourceFont *font);
    EXPORT void setColor(unsigned char r, unsigned char g, unsigned char b);
    EXPORT void setText(std::string string);
    EXPORT std::string getText();
    EXPORT void rebuildString();
    EXPORT void setShader(Shader *shader);

    EXPORT int getWidth();
    EXPORT int getHeight();

    EXPORT Matrix4 getLocalspaceMatrix() override;

protected:
    unsigned char r = 255, g = 255, b = 255;
    std::string string = "";
    bool isStringDirty = true;
    float opacity = 1.0f;
    Matrix4 mAnchor;
    ResourceFont *font = nullptr;
    Texture *texture = nullptr;

    int textTextureWidth = 0, textTextureHeight = 0;
    Shader *shader = nullptr;
    MeshStatic *mesh = nullptr;
};
