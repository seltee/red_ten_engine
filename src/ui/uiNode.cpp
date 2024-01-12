// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/uiNode.h"
#include "ui/uiNodeInput.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/opengl/textureOpenGL.h"
#include "rtengine.h"
#include <SDL_ttf.h>
#include <SDL.h>

UINode::UINode(UINode *parent)
{
    this->parent = parent;
}

UINode::~UINode()
{
    clearChildren();
}

void UINode::process(float delta, float zoom)
{
    onProcess(delta, zoom);

    for (auto &child : children)
    {
        child->process(delta, zoom);
    }
}

void UINode::recalcSize(float zoom)
{
    recalcFixedSizes();
    recalcAdaptiveSizes(zoom);
    recalcPercentageSizes(contentWidth, contentHeight);
    recalcContentDimensions(zoom);
    recalcImageSize(zoom);
}

void UINode::buildChierarchy(UIChierarchyElement *current, UIChierarchyElement *root)
{
    auto positionData = calcPositioning(&current->renderData);
    for (auto &child : children)
    {
        if (child->style.getVisibility() == UIVisibility::Hidden)
            continue;
        positionData = correctChildPositioning(child, positionData, &current->renderData);

        UIRenderData childData;
        fillChildRenderData(positionData.sx, positionData.sy, child, &current->renderData, &childData);

        if (childData.position == UIPosition::Inline)
        {
            current->children.push_back(UIChierarchyElement(child, childData));
            current->realChildren.push_back(&current->children.back());
            child->buildChierarchy(&current->children.back(), root);
            positionData = shiftInlinePositioning(child, positionData);
        }
        else
        {
            root->children.push_back(UIChierarchyElement(child, childData));
            current->realChildren.push_back(&root->children.back());
            child->buildChierarchy(&root->children.back(), root);
        }
    }
}

void UINode::onProcess(float delta, float zoom)
{
}

UINode *UINode::createChildNode()
{
    auto newNode = new UINode(this);
    children.push_back(newNode);
    return newNode;
}

UINodeInput *UINode::createChildInputNode()
{
    auto newNode = new UINodeInput(this);
    children.push_back(newNode);
    return newNode;
}

void UINode::clearChildren()
{
    for (auto &node : children)
    {
        delete node;
    }
    children.clear();
}

void UINode::setText(std::string text)
{
    this->text = text;
}

std::string UINode::getText()
{
    return this->text;
}

void UINode::setPointerCallback(UIPointerCallback callback)
{
    this->pointerCallback = callback;
}

UIPointerCallback UINode::getPointerCallback()
{
    return this->pointerCallback;
}

void UINode::setUserCallbackData(void *data)
{
    this->userCallbackData = data;
}

void *UINode::getUserCallbackData()
{
    return this->userCallbackData;
}

float UINode::getScroll()
{
    if (style.getScroll() == UIScroll::None)
        scroll = 0.0f;
    else
        scroll = max(min(scroll, maxScroll), 0.0f);
    return scroll;
}

void UINode::doScroll(float scroll, float maxScroll)
{
    this->scroll += scroll;
    this->maxScroll = maxScroll;
}

void UINode::beforeRender()
{
}

void UINode::afterRender()
{
}

void UINode::recalcFixedSizes()
{
    for (auto &child : children)
    {
        child->recalcFixedSizes();
    }

    if (!style.getWidth().bIsUsingPercentage && style.getWidth().amount != 0.0f)
        contentWidth = style.getWidth().amount + style.getPaddingLeft().amount + style.getPaddingRight().amount;

    if (!style.getHeight().bIsUsingPercentage && style.getHeight().amount != 0.0f)
        contentHeight = style.getHeight().amount + style.getPaddingTop().amount + style.getPaddingBottom().amount;
}

void UINode::recalcAdaptiveSizes(float zoom)
{
    for (auto &child : children)
    {
        child->recalcAdaptiveSizes(zoom);
    }

    if (!style.getWidth().bIsUsingPercentage && style.getWidth().amount == 0.0f)
    {
        contentWidth = 0.0f;
        for (auto &child : children)
        {
            if (child->style.getPositioning() == UIPosition::Inline)
                contentWidth += child->getContentWidth() + child->style.getMarginLeft().amount + child->style.getMarginRight().amount;
        }
        contentWidth = max(contentWidth, UIChierarchyElement::getTextWidth(static_cast<int>(style.getFontSize() * zoom), text) / zoom);
        contentWidth += style.getPaddingLeft().amount + style.getPaddingRight().amount;

        if (style.getImage() && style.getImagePositioning() == UIImagePositioning::Place)
            contentWidth = max(contentWidth, style.getImage()->getWidth() / zoom);
    }

    if (!style.getHeight().bIsUsingPercentage && style.getHeight().amount == 0.0f)
    {
        contentHeight = 0.0f;
        for (auto &child : children)
        {
            if (child->style.getPositioning() == UIPosition::Inline)
                contentHeight += child->getContentHeight() + child->style.getMarginTop().amount + child->style.getMarginBottom().amount;
        }
        contentHeight = max(contentHeight, UIChierarchyElement::getTextHeight(static_cast<int>(style.getFontSize() * zoom), text) / zoom);
        contentHeight += style.getPaddingTop().amount + style.getPaddingBottom().amount;

        if (style.getImage() && style.getImagePositioning() == UIImagePositioning::Place)
            contentHeight = max(contentHeight, style.getImage()->getHeight() / zoom);
    }
}

void UINode::recalcPercentageSizes(float proposedWidth, float proposedHeight)
{
    float childrenPercentageWidth = 0.0f;
    float childrenPercentageHeight = 0.0f;
    float contentStaticWidth = 0.0f;
    float contentStaticHeight = 0.0f;

    if (style.getWidth().bIsUsingPercentage)
        contentWidth = proposedWidth;
    if (style.getHeight().bIsUsingPercentage)
        contentHeight = proposedHeight;

    for (auto &child : children)
    {
        if (child->style.getPositioning() == UIPosition::Inline)
        {
            if (child->style.getWidth().bIsUsingPercentage)
            {
                childrenPercentageWidth += child->style.getWidth().amount / 100.0f;
                contentStaticWidth += child->style.getPaddingLeft().amount + child->style.getPaddingRight().amount;
            }
            else
            {
                contentStaticWidth += child->getContentWidth();
            }

            if (child->style.getHeight().bIsUsingPercentage)
            {
                childrenPercentageHeight += child->style.getHeight().amount / 100.0f;
                contentStaticWidth += child->style.getPaddingTop().amount + child->style.getPaddingBottom().amount;
            }
            else
            {
                contentStaticHeight += child->getContentHeight();
            }
        }
    }
    if (childrenPercentageWidth == 0.0f)
        childrenPercentageWidth = 1.0f;

    if (childrenPercentageHeight == 0.0f)
        childrenPercentageHeight = 1.0f;

    float spaceLeftWidth = max(contentWidth - contentStaticWidth, 0);
    float spaceLeftHeight = max(contentHeight - contentStaticHeight, 0);

    for (auto &child : children)
    {
        float sizeWidth = child->getContentWidth();
        float sizeHeight = child->getContentHeight();
        if (child->style.getPositioning() == UIPosition::Inline)
        {
            if (child->style.getWidth().bIsUsingPercentage)
                sizeWidth = style.getLayoutDirection() == UILayoutDirection::Horizontal ? spaceLeftWidth * ((child->style.getWidth().amount / 100.0f) / childrenPercentageWidth)
                                                                                        : contentWidth * (child->style.getWidth().amount / 100.0f);

            if (child->style.getHeight().bIsUsingPercentage)
                sizeHeight = style.getLayoutDirection() == UILayoutDirection::Vertical ? spaceLeftHeight * ((child->style.getHeight().amount / 100.0f) / childrenPercentageHeight)
                                                                                       : contentHeight * (child->style.getHeight().amount / 100.0f);

            child->recalcPercentageSizes(sizeWidth, sizeHeight);
        }
        else
        {
            if (child->style.getWidth().bIsUsingPercentage)
                sizeWidth = contentWidth * (child->style.getWidth().amount / 100.0f);
            if (child->style.getHeight().bIsUsingPercentage)
                sizeHeight = contentHeight * (child->style.getHeight().amount / 100.0f);
            child->recalcPercentageSizes(sizeWidth, sizeHeight);
        }
    }
}

void UINode::recalcImageSize(float zoom)
{
    if (style.getImage())
    {
        imageX = 0.0f;
        imageY = 0.0f;
        if (style.getImagePositioning() == UIImagePositioning::Place)
        {
            imageWidth = contentWidth * style.getImageZoom();
            imageHeight = contentHeight * style.getImageZoom();
        }
        if (style.getImagePositioning() == UIImagePositioning::PlaceBackground)
        {
            imageWidth = style.getImage()->getWidth() / zoom * style.getImageZoom();
            imageHeight = style.getImage()->getHeight() / zoom * style.getImageZoom();
        }
        if (style.getImagePositioning() == UIImagePositioning::Contain)
        {
            imageWidth = contentWidth;
            imageHeight = contentWidth * ((style.getImage()->getHeight() / zoom) / (style.getImage()->getWidth() / zoom));
            if (imageHeight > contentHeight)
            {
                imageHeight = contentHeight;
                imageWidth = contentHeight * ((style.getImage()->getWidth() / zoom) / (style.getImage()->getHeight() / zoom));
            }
            imageWidth *= style.getImageZoom();
            imageHeight *= style.getImageZoom();
            imageX = (contentWidth - imageWidth) / 2.0f;
            imageY = (contentHeight - imageHeight) / 2.0f;
        }
        if (style.getImagePositioning() == UIImagePositioning::Cover)
        {
            imageWidth = contentWidth;
            imageHeight = contentWidth * ((style.getImage()->getHeight() / zoom) / (style.getImage()->getWidth() / zoom));
            if (imageHeight < contentHeight)
            {
                imageHeight = contentHeight;
                imageWidth = contentHeight * ((style.getImage()->getWidth() / zoom) / (style.getImage()->getHeight() / zoom));
            }
            imageWidth *= style.getImageZoom();
            imageHeight *= style.getImageZoom();
            imageX = (contentWidth - imageWidth) / 2.0f;
            imageY = (contentHeight - imageHeight) / 2.0f;
        }
    }

    for (auto &child : children)
    {
        child->recalcImageSize(zoom);
    }
}

void UINode::recalcContentDimensions(float zoom)
{
    contentDimensionsWidth = 0.0f;
    contentDimensionsHeight = 0.0f;

    // size of inline objects
    for (auto &child : children)
    {
        if (child->style.getPositioning() == UIPosition::Inline)
        {
            if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
            {
                contentDimensionsWidth += child->getContentWidth();
                contentDimensionsHeight = max(contentDimensionsHeight, child->getContentHeight());
            }
            else
            {
                contentDimensionsWidth = max(contentDimensionsWidth, child->getContentWidth());
                contentDimensionsHeight += child->getContentHeight();
            }
        }
    }

    // size of absolute width & height
    for (auto &child : children)
    {
        if (child->style.getPositioning() == UIPosition::Absolute)
        {
            contentDimensionsWidth = max(contentDimensionsWidth, child->getContentWidth() - child->style.getMarginLeft().amount + child->style.getMarginRight().amount);
            contentDimensionsHeight = max(contentDimensionsHeight, child->getContentHeight() - child->style.getMarginTop().amount + child->style.getMarginBottom().amount);
        }
    }

    for (auto &child : children)
    {
        child->recalcContentDimensions(zoom);
    }
}

UINodePositioning UINode::calcPositioning(UIRenderData *renderData)
{
    UINodePositioning out;

    // render data already contains margin, so add only padding for the children elements shifts
    float sx = renderData->x + style.getPaddingLeft().amount;
    float sy = renderData->y + style.getPaddingTop().amount;

    float contentWidth = 0.0f;
    float contentHeight = 0.0f;
    for (auto &child : children)
    {
        if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
        {
            contentWidth += child->getContentWidth();
            contentHeight = max(contentHeight, child->getContentHeight());
        }
        else
        {
            contentWidth = max(contentWidth, child->getContentWidth());
            contentHeight += child->getContentHeight();
        }
    }
    float leftSpaceH = renderData->width - contentWidth - style.getPaddingLeft().amount - style.getPaddingRight().amount;
    float leftSpaceV = renderData->height - contentHeight - style.getPaddingTop().amount - style.getPaddingBottom().amount;
    float betweenH = 0.0f;
    float betweenV = 0.0f;
    int childrenCount = 0;
    for (auto &child : children)
    {
        if (child->style.getVisibility() == UIVisibility::Hidden || child->style.getPositioning() != UIPosition::Inline)
            continue;
        childrenCount++;
    }
    childrenCount = max(childrenCount, 1);

    if (style.getContentHorizontalAlign() == UIContentAlign::Middle)
        sx += leftSpaceH / 2.0f;
    if (style.getContentVerticalAlign() == UIContentAlign::Middle)
        sy += leftSpaceV / 2.0f;
    if (style.getContentHorizontalAlign() == UIContentAlign::End)
        sx += leftSpaceH;
    if (style.getContentVerticalAlign() == UIContentAlign::End)
        sy += leftSpaceV;
    if (style.getContentHorizontalAlign() == UIContentAlign::SpaceBetween)
        betweenH = childrenCount > 1 ? leftSpaceH / static_cast<float>(childrenCount - 1) : 0.0f;
    if (style.getContentVerticalAlign() == UIContentAlign::SpaceBetween)
        betweenV = childrenCount > 1 ? leftSpaceV / static_cast<float>(childrenCount - 1) : 0.0f;
    if (style.getContentHorizontalAlign() == UIContentAlign::SpaceAround)
    {
        betweenH = leftSpaceH / static_cast<float>(childrenCount + 1);
        sx += betweenH;
    }
    if (style.getContentVerticalAlign() == UIContentAlign::SpaceAround)
    {
        betweenV = leftSpaceV / static_cast<float>(childrenCount + 1);
        sy += betweenV;
    }

    out.sx = sx;
    out.sy = sy;
    out.betweenH = betweenH;
    out.betweenV = betweenV;

    return out;
}

UINodePositioning UINode::correctChildPositioning(UINode *child, UINodePositioning &positioning, UIRenderData *renderData)
{
    UINodePositioning out = positioning;
    if (style.getContentHorizontalAlign() == UIContentAlign::Middle && style.getLayoutDirection() == UILayoutDirection::Vertical)
        out.sx = renderData->x + style.getPaddingLeft().amount + (renderData->width - child->getContentWidth()) / 2.0f;
    if (style.getContentVerticalAlign() == UIContentAlign::Middle && style.getLayoutDirection() == UILayoutDirection::Horizontal)
        out.sy = renderData->y + style.getPaddingTop().amount + (renderData->height - child->getContentHeight()) / 2.0f;
    return out;
}

UINodePositioning UINode::shiftInlinePositioning(UINode *child, UINodePositioning &positioning)
{
    UINodePositioning out = positioning;
    if (style.getLayoutDirection() == UILayoutDirection::Horizontal && child->style.getPositioning() == UIPosition::Inline)
        out.sx += child->getContentWidth() + out.betweenH;
    if (style.getLayoutDirection() == UILayoutDirection::Vertical && child->style.getPositioning() == UIPosition::Inline)
        out.sy += child->getContentHeight() + out.betweenV;
    return out;
}

void UINode::fillChildRenderData(float posX, float posY, UINode *child, UIRenderData *ownData, UIRenderData *childData)
{
    float marginLeft = child->style.getMarginLeft().amount;
    float marginTop = child->style.getMarginTop().amount;

    // Colors
    childData->background = child->style.getBackgroundColor();
    childData->x = posX + marginLeft;
    childData->y = posY + marginTop - getScroll();
    childData->view = ownData->view;
    childData->imageShader = ownData->imageShader;
    childData->colorShader = ownData->colorShader;
    childData->colorShaderParameter = ownData->colorShaderParameter;
    childData->textColorShader = ownData->textColorShader;
    childData->colorTextShaderParameter = ownData->colorTextShaderParameter;
    childData->text = child->getText();
    childData->textColor = child->style.getTextColor();
    childData->zoom = ownData->zoom;
    childData->textHorizontalAlign = child->style.getTextHorizontalAlign();
    childData->textVerticalAlign = child->style.getTextVerticalAlign();
    childData->padding[0] = child->style.getPaddingLeft().amount;
    childData->padding[1] = child->style.getPaddingTop().amount;
    childData->padding[2] = child->style.getPaddingRight().amount;
    childData->padding[3] = child->style.getPaddingBottom().amount;
    childData->width = child->getContentWidth();
    childData->height = child->getContentHeight();
    childData->position = child->style.getPositioning();
    childData->contentHorizontalAlign = child->style.getContentHorizontalAlign();
    childData->contentVerticalAlign = child->style.getContentVerticalAlign();
    childData->scroll = child->style.getScroll();
    childData->contentDimensionsWidth = child->getContentDimensionsWidth();
    childData->contentDimensionsHeight = child->getContentDimensionsHeight();
    childData->fontSize = child->style.getFontSize();
    childData->image = child->style.getImage();
    childData->imageAlpha = child->style.getImageAlpha();
    childData->imageX = child->getImageX() + childData->x;
    childData->imageY = child->getImageY() + childData->y;
    childData->imageWidth = child->getImageWidth();
    childData->imageHeight = child->getImageHeight();
    childData->zIndex = child->style.getZIndex();

    if (child->style.getOverflowVisibility() == UIVisibility::Visible)
    {
        childData->viewport = ownData->viewport;
    }
    else
    {
        childData->viewport.x = max(ownData->viewport.x, childData->x);
        childData->viewport.y = max(ownData->viewport.y, childData->y);
        childData->viewport.endX = min(ownData->viewport.endX, childData->x + childData->width * childData->zoom);
        childData->viewport.endY = min(ownData->viewport.endY, childData->y + childData->height * childData->zoom);
    }
}
