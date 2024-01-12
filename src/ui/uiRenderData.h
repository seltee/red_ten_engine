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

class UIRenderData
{
public:
    float width;
    float height;
    float x;
    float y;

    // left, top, right, bottom
    float padding[4];

    float zoom;

    std::string text;
    Color textColor;

    Color background;

    Matrix4 *view;

    Shader *imageShader;

    Shader *colorShader;
    ShaderParameter *colorShaderParameter;

    Shader *textColorShader;
    ShaderParameter *colorTextShaderParameter;

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

    int zIndex;
};