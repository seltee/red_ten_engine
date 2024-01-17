// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "ui/uiNode.h"
#include "ui/uiRenderElement.h"
#include "renderer/renderTarget.h"
#include "common/utils.h"
#include <algorithm>
#include <vector>

class UINodeTreeElement
{
public:
    UIRenderElement *element = nullptr;
    std::vector<UINodeTreeElement> children;

    inline void render(RenderTarget *renderTarget, UIRenderSharedData *sharedData)
    {
        if (element)
            element->render(renderTarget, sharedData);

        for (auto &child : children)
        {
            child.render(renderTarget, sharedData);
        }
    }

    inline void sort()
    {
        std::sort(children.begin(), children.end(),
             [](const UINodeTreeElement &a, const UINodeTreeElement &b)
             {
                 if (a.element->renderData->position == UIPosition::Inline)
                     return false;
                 else
                     return b.element->renderData->zIndex > a.element->renderData->zIndex;
             });
    }

    bool propagateScroll(float mouseX, float mouseY, float zoom, float scroll);
    bool pickPoint(float x, float y, float zoom, std::vector<UIMousePickELement> *collection);
};