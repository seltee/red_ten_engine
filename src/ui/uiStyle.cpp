// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/uiStyle.h"

void UIStyle::setWidthPoints(float width)
{
    this->width.amount = width;
    this->width.bIsUsingPercentage = false;
}

void UIStyle::setHeightPoints(float height)
{
    this->height.amount = height;
    this->height.bIsUsingPercentage = false;
}

void UIStyle::setAdaptiveWidth()
{
    this->width.amount = 0.0f;
    this->width.bIsUsingPercentage = false;
}

void UIStyle::setAdaptiveHeight()
{
    this->height.amount = 0.0f;
    this->height.bIsUsingPercentage = false;
}

void UIStyle::setWidthPercentage(float widthPercentage)
{
    this->width.amount = widthPercentage;
    this->width.bIsUsingPercentage = true;
}

void UIStyle::setHeightPercentage(float heightPercentage)
{
    this->height.amount = heightPercentage;
    this->height.bIsUsingPercentage = true;
}

void UIStyle::setPaddingLeftPoints(float amount)
{
    this->padding.left = amount;
}

void UIStyle::setPaddingTopPoints(float amount)
{
    this->padding.top = amount;
}

void UIStyle::setPaddingRightPoints(float amount)
{
    this->padding.right = amount;
}

void UIStyle::setPaddingBottomPoints(float amount)
{
    this->padding.bottom = amount;
}

void UIStyle::setPaddingPoints(float left, float top, float right, float bottom)
{
    this->padding = {left, top, right, bottom};
}

void UIStyle::setMarginLeftPoints(float amount)
{
    this->margin.left = amount;
}

void UIStyle::setMarginTopPoints(float amount)
{
    this->margin.top = amount;
}

void UIStyle::setMarginRightPoints(float amount)
{
    this->margin.right = amount;
}

void UIStyle::setMarginBottomPoints(float amount)
{
    this->margin.bottom = amount;
}

void UIStyle::setMarginPoints(float left, float top, float right, float bottom)
{
    this->margin = {left, top, right, bottom};
}

void UIStyle::setBorderLeftPoints(float amount)
{
    this->border.left = amount;
}

void UIStyle::setBorderTopPoints(float amount)
{
    this->border.top = amount;
}

void UIStyle::setBorderRightPoints(float amount)
{
    this->border.right = amount;
}

void UIStyle::setBorderBottomPoints(float amount)
{
    this->border.bottom = amount;
}

void UIStyle::setBorderPoints(float left, float top, float right, float bottom)
{
    this->border = {left, top, right, bottom};
}

void UIStyle::setBackgroundColor(Color color)
{
    this->backgroundColor = color;
}

void UIStyle::setBorderColor(Color color)
{
    this->borderColor = color;
}

void UIStyle::setTextColor(Color color)
{
    this->textColor = color;
}

void UIStyle::setLayoutDirection(UILayoutDirection direction)
{
    this->direction = direction;
}

void UIStyle::setTextHorizontalAlign(UITextAlign align)
{
    this->textHorizontalAlign = align;
}

void UIStyle::setTextVerticalAlign(UITextAlign align)
{
    this->textVerticalAlign = align;
}

void UIStyle::setContentHorizontalAlign(UIContentAlign align)
{
    this->contentHorizontalAlign = align;
}

void UIStyle::setContentVerticalAlign(UIContentAlign align)
{
    this->contentVerticalAlign = align;
}

void UIStyle::setHoverCursor(UICursor cursor)
{
    this->hoverCursor = cursor;
}

UICursor UIStyle::getHoverCursor()
{
    return this->hoverCursor;
}

void UIStyle::setPositioning(UIPosition positioning)
{
    this->positioning = positioning;
}

UIPosition UIStyle::getPositioning()
{
    return positioning;
}

void UIStyle::setVisibility(UIVisibility visibility)
{
    this->visibility = visibility;
}

void UIStyle::setOverflowVisibility(UIVisibility visibility)
{
    this->overflawVisibility = visibility;
}

void UIStyle::setScroll(UIScroll scroll)
{
    this->scroll = scroll;
}

void UIStyle::setFontSize(int size)
{
    this->fontSize = size;
}

void UIStyle::setImage(Texture *image)
{
    this->image = image;
}

void UIStyle::setImagePosition(UIImagePositioning imagePosition)
{
    this->imagePosition = imagePosition;
}

void UIStyle::setImageZoom(float imageZoom)
{
    this->imageZoom = imageZoom;
}

void UIStyle::setImageAlpha(float imageAlpha)
{
    this->imageAlpha = imageAlpha;
}

void UIStyle::setImageShiftX(float imageShiftX)
{
    this->imageShiftX = imageShiftX;
}

void UIStyle::setImageShiftY(float imageShiftY)
{
    this->imageShiftY = imageShiftY;
}

void UIStyle::setImageFrameWidth(float imageFrameWidth)
{
    this->imageFrameWidth = imageFrameWidth;
}

void UIStyle::setImageFrameHeight(float imageFrameHeight)
{
    this->imageFrameHeight = imageFrameHeight;
}

void UIStyle::setZIndex(int zIndex)
{
    this->zIndex = zIndex;
}