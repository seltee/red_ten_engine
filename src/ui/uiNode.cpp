// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/uiNode.h"
#include "ui/uiNodeInput.h"
#include "ui/UINodeTreeElement.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/opengl/textureOpenGL.h"
#include "rtengine.h"
#include <SDL_ttf.h>
#include <SDL.h>

UINode::UINode(UINode *parent)
{
    this->parent = parent;
    renderElement.node = this;
    renderElement.renderData = &renderData;
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

void UINode::buildChierarchy(UIRenderPositioning positioning, UIRenderSharedData *renderSharedData, UINodeTreeElement *treeElement, UINodeTreeElement *root)
{
    // style to data
    fillRenderData(positioning, renderSharedData);
    auto positionData = calcPositioning();

    // render elements contains link to render data
    treeElement->element = &renderElement;

    for (auto &child : children)
    {
        if (child->style.getVisibility() == UIVisibility::Hidden)
            continue;
        positionData = correctChildPositioning(child, positionData);

        UIRenderPositioning childPositioning;
        childPositioning.sx = positionData.sx;
        childPositioning.sy = positionData.sy;
        childPositioning.viewport = renderData.viewport;

        if (child->style.getPositioning() == UIPosition::Inline)
        {
            treeElement->children.push_back(UINodeTreeElement());
            UINodeTreeElement *element = &treeElement->children.back();

            child->buildChierarchy(childPositioning, renderSharedData, element, root);

            positionData = shiftInlinePositioning(child, positionData);
        }
        else
        {
            root->children.push_back(UINodeTreeElement());
            UINodeTreeElement *element = &root->children.back();
            child->buildChierarchy(childPositioning, renderSharedData, element, element);
        }
    }
    treeElement->sort();
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
            {
                if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
                    contentWidth += child->getContentWidth() + child->style.getMarginLeft().amount + child->style.getMarginRight().amount;
                else
                    contentWidth = max(contentWidth, child->getContentWidth() + child->style.getMarginLeft().amount + child->style.getMarginRight().amount);
            }
        }
        contentWidth = max(contentWidth, UIRenderElement::getTextWidth(static_cast<int>(style.getFontSize() * zoom), text) / zoom);
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
            {
                if (style.getLayoutDirection() == UILayoutDirection::Vertical)
                    contentHeight += child->getContentHeight() + child->style.getMarginTop().amount + child->style.getMarginBottom().amount;
                else
                    contentHeight = max(contentHeight, child->getContentHeight() + child->style.getMarginTop().amount + child->style.getMarginBottom().amount);
            }
        }
        contentHeight = max(contentHeight, UIRenderElement::getTextHeight(static_cast<int>(style.getFontSize() * zoom), text) / zoom);
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
                contentStaticWidth += child->style.getPaddingLeft().amount + child->style.getPaddingRight().amount + child->style.getMarginLeft().amount + child->style.getMarginRight().amount;
            }
            else
            {
                contentStaticWidth += child->getContentWidth();
            }

            if (child->style.getHeight().bIsUsingPercentage)
            {
                childrenPercentageHeight += child->style.getHeight().amount / 100.0f;
                contentStaticHeight += child->style.getPaddingTop().amount + child->style.getPaddingBottom().amount + child->style.getMarginTop().amount + child->style.getMarginBottom().amount;
            }
            else
            {
                contentStaticHeight += child->getContentHeight();
            }
        }
    }
    childrenPercentageWidth = max(childrenPercentageWidth, 1.0f);
    childrenPercentageHeight = max(childrenPercentageHeight, 1.0f);

    if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
        childrenPercentageHeight = 1.0f;
    else
        childrenPercentageWidth = 1.0f;

    float spaceLeftWidth = max(contentWidth - contentStaticWidth, 0);
    float spaceLeftHeight = max(contentHeight - contentStaticHeight, 0);

    for (auto &child : children)
    {
        float sizeWidth = child->getContentWidth();
        float sizeHeight = child->getContentHeight();
        if (child->style.getPositioning() == UIPosition::Inline)
        {
            if (child->style.getWidth().bIsUsingPercentage)
            {
                if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
                    sizeWidth = spaceLeftWidth * ((child->style.getWidth().amount / 100.0f) / childrenPercentageWidth);
                else
                    sizeWidth = (contentWidth - child->style.getMarginLeft().amount - child->style.getMarginRight().amount) * (child->style.getWidth().amount / 100.0f);
            }

            if (child->style.getHeight().bIsUsingPercentage)
            {
                if (style.getLayoutDirection() == UILayoutDirection::Vertical)
                    sizeHeight = spaceLeftHeight * ((child->style.getHeight().amount / 100.0f) / childrenPercentageHeight);
                else
                    sizeHeight = (contentHeight - child->style.getMarginTop().amount - child->style.getMarginBottom().amount) * (child->style.getHeight().amount / 100.0f);
            }

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
            imageWidth = style.getImage()->getWidth() * style.getImageZoom();
            imageHeight = style.getImage()->getHeight() * style.getImageZoom();
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

void UINode::fillRenderData(UIRenderPositioning &positition, UIRenderSharedData *renderSharedData)
{
    float marginLeft = style.getMarginLeft().amount;
    float marginTop = style.getMarginTop().amount;

    // Colors
    renderData.x = positition.sx + marginLeft;
    renderData.y = positition.sy + marginTop - getScroll();
    renderData.text = this->getText();
    renderData.background = style.getBackgroundColor();
    renderData.text = getText();
    renderData.textColor = style.getTextColor();
    renderData.textHorizontalAlign = style.getTextHorizontalAlign();
    renderData.textVerticalAlign = style.getTextVerticalAlign();
    renderData.padding[0] = style.getPaddingLeft().amount;
    renderData.padding[1] = style.getPaddingTop().amount;
    renderData.padding[2] = style.getPaddingRight().amount;
    renderData.padding[3] = style.getPaddingBottom().amount;
    renderData.width = getContentWidth();
    renderData.height = getContentHeight();
    renderData.position = style.getPositioning();
    renderData.contentHorizontalAlign = style.getContentHorizontalAlign();
    renderData.contentVerticalAlign = style.getContentVerticalAlign();
    renderData.scroll = style.getScroll();
    renderData.contentDimensionsWidth = getContentDimensionsWidth();
    renderData.contentDimensionsHeight = getContentDimensionsHeight();
    renderData.fontSize = style.getFontSize();
    renderData.image = style.getImage();
    renderData.imageAlpha = style.getImageAlpha();
    renderData.imageX = getImageX() + renderData.x;
    renderData.imageY = getImageY() + renderData.y;
    renderData.imageWidth = getImageWidth();
    renderData.imageHeight = getImageHeight();
    renderData.imageShift[0] = style.getImage() ? style.getImageShiftX() / style.getImage()->getWidth() : 0.0f;
    renderData.imageShift[1] = style.getImage() ? style.getImageShiftY() / style.getImage()->getHeight() : 0.0f;
    renderData.imageFrame[0] = getImageWidth(this);
    renderData.imageFrame[1] = getImageHeight(this);
    renderData.zIndex = style.getZIndex();

    if (style.getOverflowVisibility() == UIVisibility::Visible)
    {
        renderData.viewport = positition.viewport;
    }
    else
    {
        renderData.viewport.x = max(positition.viewport.x, renderData.x * renderSharedData->zoom);
        renderData.viewport.y = max(positition.viewport.y, renderData.y * renderSharedData->zoom);
        renderData.viewport.endX = min(positition.viewport.endX, (renderData.x + renderData.width) * renderSharedData->zoom);
        renderData.viewport.endY = min(positition.viewport.endY, (renderData.y + renderData.height + getScroll()) * renderSharedData->zoom);
    }
}

UINodePositioning UINode::calcPositioning()
{
    UINodePositioning out;

    // render data already contains margin, so add only padding for the children elements shifts
    float sx = renderData.x + style.getPaddingLeft().amount;
    float sy = renderData.y + style.getPaddingTop().amount;

    float contentWidth = 0.0f;
    float contentHeight = 0.0f;
    for (auto &child : children)
    {
        if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
        {
            contentWidth += child->getContentWidth() + child->style.getMarginLeft().amount + child->style.getMarginRight().amount;
            contentHeight = max(contentHeight, child->getContentHeight());
        }
        else
        {
            contentWidth = max(contentWidth, child->getContentWidth());
            contentHeight += child->getContentHeight() + child->style.getMarginTop().amount + child->style.getMarginBottom().amount;
        }
    }
    float leftSpaceH = renderData.width - contentWidth - style.getPaddingLeft().amount - style.getPaddingRight().amount;
    float leftSpaceV = renderData.height - contentHeight - style.getPaddingTop().amount - style.getPaddingBottom().amount;
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

UINodePositioning UINode::correctChildPositioning(UINode *child, UINodePositioning &positioning)
{
    UINodePositioning out = positioning;
    if (style.getContentHorizontalAlign() == UIContentAlign::Middle && style.getLayoutDirection() == UILayoutDirection::Vertical)
        out.sx = renderData.x + style.getPaddingLeft().amount + (renderData.width - child->getContentWidth()) / 2.0f;
    if (style.getContentVerticalAlign() == UIContentAlign::Middle && style.getLayoutDirection() == UILayoutDirection::Horizontal)
        out.sy = renderData.y + style.getPaddingTop().amount + (renderData.height - child->getContentHeight()) / 2.0f;
    return out;
}

UINodePositioning UINode::shiftInlinePositioning(UINode *child, UINodePositioning &positioning)
{
    UINodePositioning out = positioning;
    if (style.getLayoutDirection() == UILayoutDirection::Horizontal && child->style.getPositioning() == UIPosition::Inline)
        out.sx += child->getContentWidth() + child->style.getMarginLeft().amount + child->style.getMarginRight().amount + out.betweenH;
    if (style.getLayoutDirection() == UILayoutDirection::Vertical && child->style.getPositioning() == UIPosition::Inline)
        out.sy += child->getContentHeight() + child->style.getMarginTop().amount + child->style.getMarginBottom().amount + out.betweenV;
    return out;
}

float UINode::getImageWidth(UINode *child)
{
    if (child->style.getImage())
    {
        if (child->style.getImageFrameWidth() == 0.0f)
            return 1.0f;
        else
            return child->style.getImageFrameWidth() / child->style.getImage()->getWidth();
    }
    return 1.0f;
}

float UINode::getImageHeight(UINode *child)
{
    if (child->style.getImage())
    {
        if (child->style.getImageFrameHeight() == 0.0f)
            return 1.0f;
        else
            return child->style.getImageFrameHeight() / child->style.getImage()->getHeight();
    }
    return 1.0f;
}
