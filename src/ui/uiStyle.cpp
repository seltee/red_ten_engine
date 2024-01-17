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
    this->paddingLeft.amount = amount;
    this->paddingLeft.bIsUsingPercentage = false;
}

void UIStyle::setPaddingLeftPercentage(float amount)
{
    this->paddingLeft.amount = amount;
    this->paddingLeft.bIsUsingPercentage = true;
}

void UIStyle::setPaddingTopPoints(float amount)
{
    this->paddingTop.amount = amount;
    this->paddingTop.bIsUsingPercentage = false;
}

void UIStyle::setPaddingTopPercentage(float amount)
{
    this->paddingTop.amount = amount;
    this->paddingTop.bIsUsingPercentage = true;
}

void UIStyle::setPaddingRightPoints(float amount)
{
    this->paddingRight.amount = amount;
    this->paddingRight.bIsUsingPercentage = false;
}

void UIStyle::setPaddingRightPercentage(float amount)
{
    this->paddingRight.amount = amount;
    this->paddingRight.bIsUsingPercentage = true;
}

void UIStyle::setPaddingBottomPoints(float amount)
{
    this->paddingBottom.amount = amount;
    this->paddingBottom.bIsUsingPercentage = false;
}

void UIStyle::setPaddingBottomPercentage(float amount)
{
    this->paddingBottom.amount = amount;
    this->paddingBottom.bIsUsingPercentage = true;
}

void UIStyle::setPaddingPoints(float left, float top, float right, float bottom)
{
    setPaddingLeftPoints(left);
    setPaddingTopPoints(top);
    setPaddingRightPoints(right);
    setPaddingBottomPoints(bottom);
}

void UIStyle::setPaddingPercentage(float left, float top, float right, float bottom)
{
    setPaddingLeftPercentage(left);
    setPaddingTopPercentage(top);
    setPaddingRightPercentage(right);
    setPaddingBottomPercentage(bottom);
}

void UIStyle::setMarginLeftPoints(float amount)
{
    this->marginLeft.amount = amount;
    this->marginLeft.bIsUsingPercentage = false;
}

void UIStyle::setMarginLeftPercentage(float amount)
{
    this->marginLeft.amount = amount;
    this->marginLeft.bIsUsingPercentage = true;
}

void UIStyle::setMarginTopPoints(float amount)
{
    this->marginTop.amount = amount;
    this->marginTop.bIsUsingPercentage = false;
}

void UIStyle::setMarginTopPercentage(float amount)
{
    this->marginTop.amount = amount;
    this->marginTop.bIsUsingPercentage = true;
}

void UIStyle::setMarginRightPoints(float amount)
{
    this->marginRight.amount = amount;
    this->marginRight.bIsUsingPercentage = false;
}

void UIStyle::setMarginRightPercentage(float amount)
{
    this->marginRight.amount = amount;
    this->marginRight.bIsUsingPercentage = true;
}

void UIStyle::setMarginBottomPoints(float amount)
{
    this->marginBottom.amount = amount;
    this->marginBottom.bIsUsingPercentage = false;
}

void UIStyle::setMarginBottomPercentage(float amount)
{
    this->marginBottom.amount = amount;
    this->marginBottom.bIsUsingPercentage = true;
}

void UIStyle::setMarginPoints(float left, float top, float right, float bottom)
{
    setMarginLeftPoints(left);
    setMarginTopPoints(top);
    setMarginRightPoints(right);
    setMarginBottomPoints(bottom);
}

void UIStyle::setMarginPercentage(float left, float top, float right, float bottom)
{
    setPaddingLeftPercentage(left);
    setPaddingTopPercentage(top);
    setPaddingRightPercentage(right);
    setPaddingBottomPercentage(bottom);
}

void UIStyle::setBackgroundColor(Color color)
{
    this->backgroundColor = color;
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