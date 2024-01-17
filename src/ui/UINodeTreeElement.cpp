// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/UINodeTreeElement.h"
#include "math/math.h"

bool UINodeTreeElement::propagateScroll(float mouseX, float mouseY, float zoom, float scroll)
{
    if (!element || !element->renderData || !element->node)
        return false;

    if (!(
            mouseX * zoom >= element->renderData->viewport.x &&
            mouseX * zoom < element->renderData->viewport.endX &&
            mouseY * zoom >= element->renderData->viewport.y &&
            mouseY * zoom < element->renderData->viewport.endY))
        return false;

    if (!(
            mouseX >= element->renderData->x &&
            mouseX < element->renderData->x + element->renderData->width &&
            mouseY >= element->renderData->y + element->node->getScroll() &&
            mouseY < element->renderData->y + element->renderData->height + element->node->getScroll()))
        return false;

    bool result = false;
    for (auto child = children.rbegin(); child != children.rend(); ++child)
        result = result || child->propagateScroll(mouseX, mouseY, zoom, scroll);

    if (!result && element->renderData->scroll != UIScroll::None)
    {
        element->node->doScroll(scroll, fmaxf(element->renderData->contentDimensionsHeight - element->renderData->height + element->renderData->padding[1] + element->renderData->padding[3], 0));
    }
    return element->renderData->scroll != UIScroll::None;
}

bool UINodeTreeElement::pickPoint(float x, float y, float zoom, std::vector<UIMousePickELement> *collection)
{
    if (!element || !element->renderData || !element->node)
        return false;

    if (!(
            x * zoom >= element->renderData->viewport.x &&
            x * zoom < element->renderData->viewport.endX &&
            y * zoom >= element->renderData->viewport.y &&
            y * zoom < element->renderData->viewport.endY))
        return false;

    if (!(
            x >= element->renderData->x &&
            x < element->renderData->x + element->renderData->width &&
            y >= element->renderData->y &&
            y < element->renderData->y + element->renderData->height))
        return false;

    collection->push_back({element->node, element->renderData->x, element->renderData->y});

    for (auto child = children.rbegin(); child != children.rend(); ++child)
    {
        bool result = child->pickPoint(x, y, zoom, collection);
        if (result)
            return true;
    }
    return true;
}