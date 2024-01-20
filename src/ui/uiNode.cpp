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
        if (child->style.getPositioning() == UIPosition::Absolute)
        {
            root->children.push_back(UINodeTreeElement());
            UINodeTreeElement *element = &root->children.back();
            child->buildChierarchy(childPositioning, renderSharedData, element, element);
        }
        if (child->style.getPositioning() == UIPosition::Relative)
        {
            treeElement->children.push_back(UINodeTreeElement());
            UINodeTreeElement *element = &treeElement->children.back();
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

bool UINode::isVisible()
{
    UINode *target = this;
    while (target)
    {
        if (target->style.getVisibility() == UIVisibility::Hidden)
            return false;
        target = target->parent;
    }
    return true;
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
        contentWidth = style.getWidth().amount + style.getPaddingBorderWidth() + style.getMarginWidth();

    if (!style.getHeight().bIsUsingPercentage && style.getHeight().amount != 0.0f)
        contentHeight = style.getHeight().amount + style.getPaddingBorderHeight() + style.getMarginHeight();
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
                    contentWidth += child->getContentWidth();
                else
                    contentWidth = max(contentWidth, child->getContentWidth());
            }
        }
        contentWidth = max(contentWidth, UIRenderElement::getTextWidth(static_cast<int>(style.getFontSize() * zoom), text) / zoom);
        contentWidth += style.getPaddingBorderWidth() + style.getMarginWidth();

        if (style.getImage() && style.getImagePositioning() == UIImagePositioning::Place)
            contentWidth = max(contentWidth, style.getImage()->getWidth() / zoom + style.getMarginWidth() + style.getPaddingBorderWidth());
    }

    if (!style.getHeight().bIsUsingPercentage && style.getHeight().amount == 0.0f)
    {
        contentHeight = 0.0f;
        for (auto &child : children)
        {
            if (child->style.getPositioning() == UIPosition::Inline)
            {
                if (style.getLayoutDirection() == UILayoutDirection::Vertical)
                    contentHeight += child->getContentHeight();
                else
                    contentHeight = max(contentHeight, child->getContentHeight());
            }
        }
        contentHeight = max(contentHeight, UIRenderElement::getTextHeight(static_cast<int>(style.getFontSize() * zoom), text) / zoom);
        contentHeight += style.getPaddingBorderHeight() + style.getMarginHeight();

        if (style.getImage() && style.getImagePositioning() == UIImagePositioning::Place)
            contentHeight = max(contentHeight, style.getImage()->getHeight() / zoom + style.getMarginHeight() + style.getPaddingBorderHeight());
    }
}

void UINode::recalcPercentageSizes(float proposedWidth, float proposedHeight)
{
    float childrenPercentageWidth = 0.0f;
    float childrenPercentageHeight = 0.0f;
    float contentStaticWidth = 0.0f;
    float contentStaticHeight = 0.0f;

    if (style.getWidth().bIsUsingPercentage)
        contentWidth = proposedWidth + style.getPaddingBorderWidth() + style.getMarginWidth();
    if (style.getHeight().bIsUsingPercentage)
        contentHeight = proposedHeight + style.getPaddingBorderHeight() + style.getMarginHeight();

    for (auto &child : children)
    {
        if (child->style.getPositioning() == UIPosition::Inline && child->style.getVisibility() == UIVisibility::Visible)
        {
            if (child->style.getWidth().bIsUsingPercentage)
            {
                childrenPercentageWidth += child->style.getWidth().amount / 100.0f;
                contentStaticWidth += child->style.getMarginWidth() + child->style.getPaddingBorderWidth();
            }
            else
            {
                contentStaticWidth += child->getContentWidth();
            }

            if (child->style.getHeight().bIsUsingPercentage)
            {
                childrenPercentageHeight += child->style.getHeight().amount / 100.0f;
                contentStaticHeight += child->style.getMarginHeight() + child->style.getPaddingBorderHeight();
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

    float spaceLeftWidth = max(contentWidth - style.getMarginWidth() - style.getPaddingBorderWidth() - contentStaticWidth, 0);
    float spaceLeftHeight = max(contentHeight - style.getMarginHeight() - style.getPaddingBorderHeight() - contentStaticHeight, 0);

    // real bounds of inner elements
    float innerContentWidth = contentWidth - style.getMarginWidth() - style.getPaddingBorderWidth();
    float innerContentHeight = contentHeight - style.getMarginHeight() - style.getPaddingBorderHeight();

    for (auto &child : children)
    {
        if (child->style.getVisibility() == UIVisibility::Hidden)
            continue;
        float sizeWidth = child->getContentWidth() - child->style.getMarginWidth() - child->style.getPaddingBorderWidth();
        float sizeHeight = child->getContentHeight() - child->style.getMarginHeight() - child->style.getPaddingBorderHeight();
        if (child->style.getPositioning() == UIPosition::Inline)
        {
            if (child->style.getWidth().bIsUsingPercentage)
            {
                if (style.getLayoutDirection() == UILayoutDirection::Horizontal)
                    sizeWidth = spaceLeftWidth * ((child->style.getWidth().amount / 100.0f) / childrenPercentageWidth);
                else
                    sizeWidth = (innerContentWidth - child->style.getMarginWidth() - child->style.getPaddingBorderWidth()) * (child->style.getWidth().amount / 100.0f);
            }

            if (child->style.getHeight().bIsUsingPercentage)
            {
                if (style.getLayoutDirection() == UILayoutDirection::Vertical)
                    sizeHeight = spaceLeftHeight * ((child->style.getHeight().amount / 100.0f) / childrenPercentageHeight);
                else
                    sizeHeight = (innerContentHeight - child->style.getMarginHeight() - child->style.getPaddingBorderHeight()) * (child->style.getHeight().amount / 100.0f);
            }
            child->recalcPercentageSizes(sizeWidth, sizeHeight);
        }
        else
        {
            if (child->style.getWidth().bIsUsingPercentage)
                sizeWidth = (innerContentWidth - child->style.getMarginWidth() - child->style.getPaddingBorderWidth()) * (child->style.getWidth().amount / 100.0f);
            if (child->style.getHeight().bIsUsingPercentage)
                sizeHeight = (innerContentHeight - child->style.getMarginHeight() - child->style.getPaddingBorderHeight()) * (child->style.getHeight().amount / 100.0f);

            child->recalcPercentageSizes(sizeWidth, sizeHeight);
        }
    }
}

void UINode::recalcImageSize(float zoom)
{
    if (style.getImage())
    {
        float containerWidth = contentWidth - style.getMarginWidth();
        float containerHeight = contentHeight - style.getMarginHeight();

        imageX = 0.0f;
        imageY = 0.0f;
        if (style.getImagePositioning() == UIImagePositioning::Place)
        {
            imageWidth = containerWidth * style.getImageZoom();
            imageHeight = containerHeight * style.getImageZoom();
        }
        if (style.getImagePositioning() == UIImagePositioning::PlaceBackground)
        {
            imageWidth = style.getImage()->getWidth() * style.getImageZoom();
            imageHeight = style.getImage()->getHeight() * style.getImageZoom();
        }
        if (style.getImagePositioning() == UIImagePositioning::Contain)
        {
            imageWidth = containerWidth;
            imageHeight = containerWidth * ((style.getImage()->getHeight() / zoom) / (style.getImage()->getWidth() / zoom));
            if (imageHeight > containerHeight)
            {
                imageHeight = containerHeight;
                imageWidth = containerHeight * ((style.getImage()->getWidth() / zoom) / (style.getImage()->getHeight() / zoom));
            }
            imageWidth *= style.getImageZoom();
            imageHeight *= style.getImageZoom();
            imageX = (containerWidth - imageWidth) / 2.0f;
            imageY = (containerHeight - imageHeight) / 2.0f;
        }
        if (style.getImagePositioning() == UIImagePositioning::Cover)
        {
            imageWidth = containerWidth;
            imageHeight = containerWidth * ((style.getImage()->getHeight() / zoom) / (style.getImage()->getWidth() / zoom));
            if (imageHeight < containerHeight)
            {
                imageHeight = containerHeight;
                imageWidth = containerHeight * ((style.getImage()->getWidth() / zoom) / (style.getImage()->getHeight() / zoom));
            }
            imageWidth *= style.getImageZoom();
            imageHeight *= style.getImageZoom();
            imageX = (containerWidth - imageWidth) / 2.0f;
            imageY = (containerHeight - imageHeight) / 2.0f;
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
        if (child->style.getPositioning() == UIPosition::Absolute || child->style.getPositioning() == UIPosition::Relative)
        {
            contentDimensionsWidth = max(contentDimensionsWidth, child->getContentWidth() - child->style.getMargin().left + child->style.getMargin().right);
            contentDimensionsHeight = max(contentDimensionsHeight, child->getContentHeight() - child->style.getMargin().top + child->style.getMargin().bottom);
        }
    }

    for (auto &child : children)
    {
        child->recalcContentDimensions(zoom);
    }
}

void UINode::fillRenderData(UIRenderPositioning &positition, UIRenderSharedData *renderSharedData)
{

    // Colors
    renderData.x = positition.sx;
    renderData.y = positition.sy - getScroll();
    renderData.text = this->getText();
    renderData.backgroundColor = style.getBackgroundColor();
    renderData.textColor = style.getTextColor();
    renderData.borderColor = style.getBorderColor();
    renderData.textHorizontalAlign = style.getTextHorizontalAlign();
    renderData.textVerticalAlign = style.getTextVerticalAlign();
    renderData.margin[0] = style.getMargin().left;
    renderData.margin[1] = style.getMargin().top;
    renderData.margin[2] = style.getMargin().right;
    renderData.margin[3] = style.getMargin().bottom;
    renderData.padding[0] = style.getPadding().left;
    renderData.padding[1] = style.getPadding().top;
    renderData.padding[2] = style.getPadding().right;
    renderData.padding[3] = style.getPadding().bottom;
    renderData.border[0] = style.getBorder().left;
    renderData.border[1] = style.getBorder().top;
    renderData.border[2] = style.getBorder().right;
    renderData.border[3] = style.getBorder().bottom;
    renderData.width = getContentWidth() - style.getMarginWidth() - style.getPaddingBorderWidth();
    renderData.height = getContentHeight() - style.getMarginHeight() - style.getPaddingBorderHeight();
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
        float width = renderData.width + renderData.padding[0] + renderData.padding[2] + renderData.border[0] + renderData.border[2];
        float height = renderData.height + renderData.padding[1] + renderData.padding[3] + renderData.border[1] + renderData.border[3];
        float zoom = renderSharedData->zoom;

        renderData.viewport.x = max(positition.viewport.x, (renderData.x + renderData.margin[0]) * zoom);
        renderData.viewport.y = max(positition.viewport.y, (renderData.y + renderData.margin[1]) * zoom);
        renderData.viewport.endX = min(positition.viewport.endX, (renderData.x + width + renderData.margin[2]) * zoom);
        renderData.viewport.endY = min(positition.viewport.endY, (renderData.y + height + renderData.margin[3] + getScroll()) * zoom);
    }
}

UINodePositioning UINode::calcPositioning()
{
    UINodePositioning out;

    // render data already contains margin, so add only padding for the children elements shifts
    float sx = renderData.x + style.getPadding().left + style.getMargin().left + style.getBorder().left;
    float sy = renderData.y + style.getPadding().top + style.getMargin().top + style.getBorder().top;

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
    float leftSpaceH = renderData.width - contentWidth;
    float leftSpaceV = renderData.height - contentHeight;
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
        out.sx = renderData.x + style.getPadding().left + style.getBorder().left + (renderData.width - child->getContentWidth()) / 2.0f;
    if (style.getContentVerticalAlign() == UIContentAlign::Middle && style.getLayoutDirection() == UILayoutDirection::Horizontal)
        out.sy = renderData.y + style.getPadding().top + style.getBorder().top + style.getMargin().top + (renderData.height + style.getMarginHeight() - child->getContentHeight()) / 2.0f;
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
