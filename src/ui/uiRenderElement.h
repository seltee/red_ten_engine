// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "ui/uiStyle.h"
#include "ui/uiRenderData.h"
#include "renderer/renderer.h"
#include "renderer/renderTarget.h"
#include "connector/withRenderer.h"
#include <string>

class UINode;

struct UITextCacheElement
{
    Texture *texture;
    std::string text;
    int fontSize;
    float textureWidth;
    float textureHeight;
};

struct UIMousePickELement
{
    UINode *node;
    float elX;
    float elY;
};

class UIRenderElement : public WithRenderer
{
public:
    UIRenderElement();
    UIRenderElement(UINode *node, UIRenderData *renderData);

    UIRenderData *renderData;
    UINode *node = nullptr;

    void render(RenderTarget *renderTarget, UIRenderSharedData *sharedData);

    static float getTextWidth(int fontSize, std::string text);
    static float getTextHeight(int fontSize, std::string text);

protected:
    static UITextCacheElement *getTextCacheElement(int fontSize, std::string text);
    static std::vector<UITextCacheElement> textTexturesCache;
};