// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/UINodeTreeElement.h"
#include "math/math.h"

bool UINodeTreeElement::propagateScroll(float mouseX, float mouseY, float zoom, float scroll)
{
    if (!element || !element->renderData || !element->node)
        return false;
    auto renderData = element->renderData;

    if (!(
            mouseX * zoom >= renderData->viewport.x &&
            mouseX * zoom < renderData->viewport.endX &&
            mouseY * zoom >= renderData->viewport.y &&
            mouseY * zoom < renderData->viewport.endY))
        return false;

    float elWidth = renderData->width + renderData->padding[0] + renderData->padding[2] + renderData->border[0] + renderData->border[2];
    float elHeight = renderData->height + renderData->padding[1] + renderData->padding[3] + renderData->border[1] + renderData->border[3];
    if (!(
            mouseX >= renderData->x + renderData->margin[0] &&
            mouseX < renderData->x + renderData->margin[0] + elWidth &&
            mouseY >= renderData->y + renderData->margin[1] + element->node->getScroll() &&
            mouseY < renderData->y + renderData->margin[1] + elHeight + element->node->getScroll()))
        return false;

    bool result = false;
    for (auto child = children.rbegin(); child != children.rend(); ++child)
        result = result || child->propagateScroll(mouseX, mouseY, zoom, scroll);

    if (!result && renderData->scroll != UIScroll::None)
    {
        element->node->doScroll(scroll, fmaxf(renderData->contentDimensionsHeight - renderData->height + renderData->padding[1] + renderData->padding[3], 0));
    }
    return renderData->scroll != UIScroll::None;
}

bool UINodeTreeElement::pickPoint(float x, float y, float zoom, std::vector<UIMousePickELement> *collection)
{
    if (!element || !element->renderData || !element->node)
        return false;
    auto renderData = element->renderData;

    if (!(
            x * zoom >= renderData->viewport.x &&
            x * zoom < renderData->viewport.endX &&
            y * zoom >= renderData->viewport.y &&
            y * zoom < renderData->viewport.endY))
        return false;

    float elWidth = renderData->width + renderData->padding[0] + renderData->padding[2] + renderData->border[0] + renderData->border[2];
    float elHeight = renderData->height + renderData->padding[1] + renderData->padding[3] + renderData->border[1] + renderData->border[3];
    if (!(
            x >= renderData->x + renderData->margin[0] &&
            x < renderData->x + +renderData->margin[0] + elWidth &&
            y >= renderData->y + renderData->margin[1] &&
            y < renderData->y + renderData->margin[1] + elHeight))
        return false;

    collection->push_back({element->node, renderData->x, renderData->y});

    for (auto child = children.rbegin(); child != children.rend(); ++child)
    {
        bool result = child->pickPoint(x, y, zoom, collection);
        if (result)
            return true;
    }
    return true;
}