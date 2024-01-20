// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/color.h"
#include "common/utils.h"
#include "math/math.h"
#include "renderer/shader.h"
#include <string>

struct UIViewport
{
    float x, y, endX, endY;
};

struct UIRenderSharedData
{
    Matrix4 *view;

    Shader *imageShader;
    ShaderParameter *imageShiftShaderParameter;
    ShaderParameter *imageFrameShaderParameter;

    Shader *colorShader;
    ShaderParameter *colorShaderParameter;

    Shader *textColorShader;
    ShaderParameter *colorTextShaderParameter;

    float zoom;
};

class UIRenderData
{
public:
    float width;
    float height;
    float x;
    float y;

    // left, top, right, bottom
    float margin[4];
    float padding[4];
    float border[4];

    std::string text;

    Color textColor;
    Color backgroundColor;
    Color borderColor;

    UITextAlign textHorizontalAlign;
    UITextAlign textVerticalAlign;

    UIPosition position;

    UIContentAlign contentHorizontalAlign;
    UIContentAlign contentVerticalAlign;

    UIViewport viewport;
    UIScroll scroll;

    float contentDimensionsWidth;
    float contentDimensionsHeight;

    int fontSize;

    Texture *image;
    float imageAlpha;
    float imageX, imageY;
    float imageWidth, imageHeight;

    float imageShift[2];
    float imageFrame[2];

    int zIndex;
};