// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/shader.h"
#include "common/utils.h"
#include "common/pawn.h"
#include "stage/layer.h"
#include "connector/withProfiler.h"
#include "connector/withRenderer.h"
#include "ui/uiNode.h"
#include "ui/uiNodeInput.h"
#include <vector>

class LayerUI : public Layer, public WithProfiler, public WithRenderer, public Pawn
{
public:
    LayerUI(std::string name, int index);
    ~LayerUI();

    EXPORT void setZoom(float zoom);

    EXPORT UINode *getRootNode();

    EXPORT void process(float delta) override;
    EXPORT void render(Renderer *renderer, RenderTarget *renderTarget) override;

protected:
    void initRenderData(UIRenderData *renderData);
    void inputMouseMove(InputType type, int deviceIndex, int index, float state);
    void inputMousePressLeft(InputType type, int deviceIndex, int index, bool state);
    void inputMousePressRight(InputType type, int deviceIndex, int index, bool state);
    void inputMouseWheelV(InputType type, int deviceIndex, int index, float state);

    UINode *rootContainer = nullptr;
    UINode *root = nullptr;
    Shader *colorShader = nullptr;
    Shader *imageShader = nullptr;
    ShaderParameter *colorShaderParameter = nullptr;

    Shader *textShader = nullptr;
    ShaderParameter *colorTextShaderParameter = nullptr;

    float zoom = 1.0f;
    float scroll = 0.0f;

    float mouseX = 10000.0f, mouseY= 10000.0f;
    bool bMousePressLeft = false, bMousePressRight = false;
    bool bMouseReleaseLeft = false, bMouseReleaseRight = false;
    int screenWidth = 640, screenHeight = 480;

    Matrix4 projection;
    std::vector<UINode *> hoveredNodes;

    UIChierarchyElement chierarchyRoot;
};
