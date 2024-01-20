// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/color.h"
#include "renderer/texture.h"

enum class UILayoutDirection
{
    Horizontal,
    Vertical,
};

enum class UITextAlign
{
    Start,
    Middle,
    End
};

enum class UIContentAlign
{
    Start,
    Middle,
    End,
    SpaceBetween,
    SpaceAround
};

enum class UICursor
{
    None,
    Default,
    Pointer,
    IBeam,
};

struct UIVariableSize
{
    float amount;
    bool bIsUsingPercentage;
};

struct UIAroundSpace
{
    float left;
    float top;
    float right;
    float bottom;
};

enum class UIPosition
{
    Inline,
    Absolute,
    Relative
};

enum class UIVisibility
{
    Visible,
    Hidden
};

enum class UIScroll
{
    None,
    Enabled,
    EnabledHidden
};

enum class UIImagePositioning
{
    Place,
    PlaceBackground,
    Cover,
    Contain
};

class UIStyle
{
public:
    EXPORT void setWidthPoints(float width);
    EXPORT void setHeightPoints(float height);
    EXPORT void setAdaptiveWidth();
    EXPORT void setAdaptiveHeight();
    EXPORT void setWidthPercentage(float widthPercentage);
    EXPORT void setHeightPercentage(float heightPercentage);
    EXPORT void setPaddingLeftPoints(float amount);
    EXPORT void setPaddingTopPoints(float amount);
    EXPORT void setPaddingRightPoints(float amount);
    EXPORT void setPaddingBottomPoints(float amount);
    EXPORT void setPaddingPoints(float left, float top, float right, float bottom);
    EXPORT void setMarginLeftPoints(float amount);
    EXPORT void setMarginTopPoints(float amount);
    EXPORT void setMarginRightPoints(float amount);
    EXPORT void setMarginBottomPoints(float amount);
    EXPORT void setMarginPoints(float left, float top, float right, float bottom);
    EXPORT void setBorderLeftPoints(float amount);
    EXPORT void setBorderTopPoints(float amount);
    EXPORT void setBorderRightPoints(float amount);
    EXPORT void setBorderBottomPoints(float amount);
    EXPORT void setBorderPoints(float left, float top, float right, float bottom);
    EXPORT void setBackgroundColor(Color color);
    EXPORT void setBorderColor(Color color);
    EXPORT void setTextColor(Color color);
    EXPORT void setLayoutDirection(UILayoutDirection direction);
    EXPORT void setTextHorizontalAlign(UITextAlign align);
    EXPORT void setTextVerticalAlign(UITextAlign align);
    EXPORT void setContentHorizontalAlign(UIContentAlign align);
    EXPORT void setContentVerticalAlign(UIContentAlign align);
    EXPORT void setHoverCursor(UICursor cursor);
    EXPORT UICursor getHoverCursor();
    EXPORT void setPositioning(UIPosition positioning);
    EXPORT UIPosition getPositioning();
    EXPORT void setVisibility(UIVisibility visibility);
    EXPORT void setOverflowVisibility(UIVisibility visibility);
    EXPORT void setScroll(UIScroll scroll);
    EXPORT void setFontSize(int size);
    EXPORT void setImage(Texture *image);
    EXPORT void setImagePosition(UIImagePositioning imagePosition);
    EXPORT void setImageZoom(float imageZoom);
    EXPORT void setImageAlpha(float imageAlpha);
    EXPORT void setImageShiftX(float imageShiftX);
    EXPORT void setImageShiftY(float imageShiftY);
    EXPORT void setImageFrameWidth(float imageFrameWidth);
    EXPORT void setImageFrameHeight(float imageFrameHeight);
    EXPORT void setZIndex(int zIndex);

    EXPORT inline UIVariableSize getWidth() { return width; }
    EXPORT inline UIVariableSize getHeight() { return height; }
    EXPORT inline UIAroundSpace getPadding() { return padding; }
    EXPORT inline UIAroundSpace getMargin() { return margin; }
    EXPORT inline UIAroundSpace getBorder() { return border; }

    EXPORT inline UITextAlign getTextHorizontalAlign() { return textHorizontalAlign; }
    EXPORT inline UITextAlign getTextVerticalAlign() { return textVerticalAlign; }

    EXPORT inline UIContentAlign getContentHorizontalAlign() { return contentHorizontalAlign; }
    EXPORT inline UIContentAlign getContentVerticalAlign() { return contentVerticalAlign; }

    EXPORT inline UIVisibility getVisibility() { return visibility; }
    EXPORT inline UIVisibility getOverflowVisibility() { return overflawVisibility; }

    EXPORT inline UILayoutDirection getLayoutDirection() { return direction; }
    EXPORT inline Color getBackgroundColor() { return backgroundColor; }
    EXPORT inline Color getTextColor() { return textColor; }
    EXPORT inline Color getBorderColor() { return borderColor; }

    EXPORT inline UIScroll getScroll() { return scroll; };
    EXPORT inline int getFontSize() { return fontSize; };

    EXPORT inline Texture *getImage() { return image; }
    EXPORT inline UIImagePositioning getImagePositioning() { return imagePosition; }
    EXPORT inline float getImageZoom() { return imageZoom; }
    EXPORT inline float getImageAlpha() { return imageAlpha; }
    EXPORT inline float getImageShiftX() { return imageShiftX; }
    EXPORT inline float getImageShiftY() { return imageShiftY; }
    EXPORT inline float getImageFrameWidth() { return imageFrameWidth; }
    EXPORT inline float getImageFrameHeight() { return imageFrameHeight; }

    EXPORT inline float getPaddingBorderWidth() { return padding.left + padding.right + border.left + border.right; }
    EXPORT inline float getPaddingBorderHeight() { return padding.top + padding.bottom + border.top + border.bottom; }
    EXPORT inline float getMarginWidth() { return margin.left + margin.right; }
    EXPORT inline float getMarginHeight() { return margin.top + margin.bottom; }

    EXPORT inline int getZIndex() { return zIndex; }

protected:
    UIVariableSize width = {0.0f, false};
    UIVariableSize height = {0.0f, false};
    UIAroundSpace margin = {0.0f, 0.0f, 0.0f, 0.0f};
    UIAroundSpace padding = {0.0f, 0.0f, 0.0f, 0.0f};
    UIAroundSpace border = {0.0f, 0.0f, 0.0f, 0.0f};

    Color backgroundColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
    Color borderColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
    Color textColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
    UILayoutDirection direction = UILayoutDirection::Horizontal;
    UITextAlign textHorizontalAlign = UITextAlign::Start;
    UITextAlign textVerticalAlign = UITextAlign::Start;

    UICursor hoverCursor = UICursor::None;
    UIPosition positioning = UIPosition::Inline;
    UIVisibility visibility = UIVisibility::Visible;
    UIVisibility overflawVisibility = UIVisibility::Visible;

    UIContentAlign contentHorizontalAlign = UIContentAlign::Start;
    UIContentAlign contentVerticalAlign = UIContentAlign::Start;

    UIScroll scroll = UIScroll::None;

    Texture *image = nullptr;
    UIImagePositioning imagePosition = UIImagePositioning::Place;
    float imageZoom = 1.0f;
    float imageAlpha = 1.0f;
    float imageShiftX = 0.0f;
    float imageShiftY = 0.0f;
    float imageFrameWidth = 0.0f;
    float imageFrameHeight = 0.0f;

    int fontSize = 16;
    int zIndex = 0;
};