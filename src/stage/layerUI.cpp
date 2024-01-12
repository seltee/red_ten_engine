// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "stage/layerUI.h"
#include "renderer/opengl/glew.h"
#include "rtengine.h"
#include <array>
#include <functional>
#include <vector>

extern const std::string uiColorFragmentShader;
extern const std::string uiFontFragmentShader;
extern const std::string uiSpriteFragmentShader;

LayerUI::LayerUI(std::string name, int index) : Layer(name, index), Pawn(this)
{
    rootContainer = new UINode(nullptr);
    root = rootContainer->createChildNode();
    auto mouseMoveInput = registerAxisCallback(&LayerUI::inputMouseMove);
    mouseMoveInput->addMouseMoveBinding(1.0f);
    auto mousePressLeftInput = registerButtonCallback(&LayerUI::inputMousePressLeft);
    mousePressLeftInput->addMouseButtonBinding(InputTypeMouse::LEFT_BUTTON, 1.0f);
    auto mousePressRightInput = registerButtonCallback(&LayerUI::inputMousePressRight);
    mousePressRightInput->addMouseButtonBinding(InputTypeMouse::RIGHT_BUTTON, 1.0f);
    auto inputMouseWheel = registerAxisCallback(&LayerUI::inputMouseWheelV);
    inputMouseWheel->addMouseMoveBinding(1.0f);
}

LayerUI::~LayerUI()
{
}

void LayerUI::setZoom(float zoom)
{
    this->zoom = zoom;
}

UINode *LayerUI::getRootNode()
{
    return root;
}

void LayerUI::process(float delta)
{
    rootContainer->style.setWidthPoints(screenWidth / zoom);
    rootContainer->style.setHeightPoints(screenHeight / zoom);
    rootContainer->style.setPositioning(UIPosition::Inline);
    root->style.setWidthPoints(screenWidth / zoom);
    root->style.setHeightPoints(screenHeight / zoom);
    root->style.setPositioning(UIPosition::Inline);
    root->style.setZIndex(-1);

    initRenderData(&chierarchyRoot.renderData);
    chierarchyRoot.children.clear();
    chierarchyRoot.node = rootContainer;

    rootContainer->process(delta, zoom);
    rootContainer->recalcSize(zoom);
    rootContainer->buildChierarchy(&chierarchyRoot, &chierarchyRoot);

    std::sort(chierarchyRoot.children.begin(), chierarchyRoot.children.end(), [](UIChierarchyElement a, UIChierarchyElement b)
              { return a.renderData.zIndex < b.renderData.zIndex; });

    std::vector<UIMousePickELement> collection;
    collection.push_back({root, 0.0f, 0.0f});

    if (scroll != 0.0f)
    {
        chierarchyRoot.propagateScroll(static_cast<int>(static_cast<float>(mouseX) / zoom), static_cast<int>(static_cast<float>(mouseY) / zoom), -scroll / zoom * 80.0f);
        scroll = 0.0f;
    }
    chierarchyRoot.pickPoint(static_cast<int>(static_cast<float>(mouseX) / zoom), static_cast<int>(static_cast<float>(mouseY) / zoom), &collection);

    UICursor cursor = UICursor::Default;

    // hover event
    for (auto element = collection.rbegin(); element != collection.rend(); element++)
    {
        auto node = element->node;
        if (node->style.getHoverCursor() != UICursor::None)
        {
            cursor = node->style.getHoverCursor();
        }

        UIPointerCallback callback = node->getPointerCallback();
        if (callback)
        {
            PointerCallbackData callbackData;
            callbackData.userData = node->getUserCallbackData();
            callbackData.relativeX = (mouseX - element->elX * zoom) / zoom;
            callbackData.relativeY = (mouseY - element->elY * zoom) / zoom;
            callbackData.globalX = mouseX / zoom;
            callbackData.globalY = mouseY / zoom;
            callbackData.value = 0.0f;
            callbackData.zoom = zoom;

            callbackData.event = UIPointerEvent::CursorMove;
            callback(&callbackData);

            if (!node->isHover)
            {
                callbackData.event = UIPointerEvent::CursorHover;
                callback(&callbackData);
                hoveredNodes.push_back(node);
                node->isHover = true;
            }

            if (bMousePressLeft)
            {
                callbackData.event = UIPointerEvent::CursorClickLeftPress;
                bMousePressLeft = bMousePressLeft && callback(&callbackData);
            }
            if (bMousePressRight)
            {
                callbackData.event = UIPointerEvent::CursorClickRightPress;
                bMousePressRight = bMousePressRight && callback(&callbackData);
            }
            if (bMouseReleaseLeft)
            {
                callbackData.event = UIPointerEvent::CursorClickLeftRelease;
                bMouseReleaseLeft = bMouseReleaseLeft && callback(&callbackData);
            }
            if (bMouseReleaseRight)
            {
                callbackData.event = UIPointerEvent::CursorClickRightRelease;
                bMouseReleaseRight = bMouseReleaseRight && callback(&callbackData);
            }
        }
    }

    // blur event
    for (auto node = hoveredNodes.begin(); node != hoveredNodes.end();)
    {
        bool found = false;
        for (auto element = collection.begin(); element != collection.end(); element++)
        {
            if (element->node == *node)
                found = true;
        }
        if (!found)
        {
            UIPointerCallback callback = (*node)->getPointerCallback();
            if (callback)
            {
                PointerCallbackData callbackData;
                callbackData.event = UIPointerEvent::CursorBlur;
                callbackData.userData = (*node)->getUserCallbackData();
                callbackData.relativeX = 0.0f;
                callbackData.relativeY = 0.0f;
                callbackData.globalX = mouseX;
                callbackData.globalY = mouseY;
                callbackData.value = 0.0f;

                callback(&callbackData);
                (*node)->isHover = false;
                node = hoveredNodes.erase(node);
                continue;
            }
        }
        node++;
    }

    auto engine = RTEngine::getInstance();
    auto viewController = engine->getViewController();
    if (cursor == UICursor::Default)
        viewController->setCursorType(CursorType::Default);
    if (cursor == UICursor::Pointer)
        viewController->setCursorType(CursorType::Hand);
    if (cursor == UICursor::IBeam)
        viewController->setCursorType(CursorType::IBeam);

    bMousePressLeft = false;
    bMousePressRight = false;
    bMouseReleaseLeft = false;
    bMouseReleaseRight = false;
}

void LayerUI::render(Renderer *renderer, RenderTarget *renderTarget)
{
    if (!colorShader)
    {
        colorShader = renderer->createOpenGLShader(uiColorFragmentShader);
        colorShaderParameter = colorShader->createShaderParameter("v4color", ShaderParameterType::Float4);
    }

    if (!textShader)
    {
        textShader = renderer->createOpenGLShader(uiFontFragmentShader);
        colorTextShaderParameter = textShader->createShaderParameter("v4color", ShaderParameterType::Float4);
    }

    if (!imageShader)
    {
        imageShader = renderer->createOpenGLShader(uiSpriteFragmentShader);
    }

    if (!chierarchyRoot.renderData.colorShader)
        return;

    screenWidth = renderTarget->getWidth();
    screenHeight = renderTarget->getHeight();

    renderTarget->useResultBuffer();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    UINodeAbsolutePosition node;
    initRenderData(&node.renderData);
    node.node = rootContainer;

    glEnable(GL_SCISSOR_TEST);
    chierarchyRoot.render(renderTarget);
    glDisable(GL_SCISSOR_TEST);
}

void LayerUI::initRenderData(UIRenderData *renderData)
{
    float width = static_cast<float>(screenWidth) / zoom;
    float height = static_cast<float>(screenHeight) / zoom;
    projection = glm::ortho(0.0f, width, height, 0.0f, 10.0f, -10.0f);

    renderData->width = width;
    renderData->height = height;
    renderData->x = 0.0f;
    renderData->y = 0.0f;

    renderData->padding[0] = 0.0f;
    renderData->padding[1] = 0.0f;
    renderData->padding[2] = 0.0f;
    renderData->padding[3] = 0.0f;

    renderData->zoom = zoom;
    renderData->background = rootContainer->style.getBackgroundColor();

    renderData->view = &projection;

    renderData->imageShader = imageShader;
    renderData->image = nullptr;
    renderData->imageX = 0.0f;
    renderData->imageY = 0.0f;
    renderData->imageWidth = 0.0f;
    renderData->imageHeight = 0.0f;
    renderData->imageAlpha = 0.0f;

    renderData->colorShader = colorShader;
    renderData->colorShaderParameter = colorShaderParameter;

    renderData->textColorShader = textShader;
    renderData->colorTextShaderParameter = colorTextShaderParameter;

    renderData->textHorizontalAlign = UITextAlign::Start;
    renderData->textVerticalAlign = UITextAlign::Start;
    renderData->position = UIPosition::Inline;

    renderData->contentHorizontalAlign = UIContentAlign::Start;
    renderData->contentVerticalAlign = UIContentAlign::Start;

    renderData->viewport = {0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};

    renderData->text = "";
    renderData->scroll = UIScroll::None;

    renderData->zIndex = 0;
}

void LayerUI::inputMouseMove(InputType type, int deviceIndex, int index, float state)
{
    if (index == (int)InputTypeMouseMove::MOVE_HORIZONTAL)
        mouseX = state;
    if (index == (int)InputTypeMouseMove::MOVE_VERTICAL)
        mouseY = state;
}

void LayerUI::inputMousePressLeft(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
        bMousePressLeft = true;
    else
        bMouseReleaseLeft = true;
}

void LayerUI::inputMousePressRight(InputType type, int deviceIndex, int index, bool state)
{
    if (state)
        bMousePressRight = true;
    else
        bMouseReleaseRight = true;
}

void LayerUI::inputMouseWheelV(InputType type, int deviceIndex, int index, float state)
{
    if (index == (int)InputTypeMouseMove::MOVE_WHEEL_VERTICAL)
        scroll += state;
}

const std::string uiColorFragmentShader =
    "#version 410 core\n"
    "out vec4 gAlbedo;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D t;\n"
    "uniform vec4 v4color;\n"
    "void main() {\n"
    "   gAlbedo = v4color;\n"
    "}\n";

const std::string uiFontFragmentShader =
    "#version 410 core\n"
    "out vec4 gAlbedo;\n"
    "uniform sampler2D tText;\n"
    "uniform vec4 v4color;\n"
    "in vec2 texCoord;\n"
    "void main() {\n"
    "   gAlbedo = texture(tText, texCoord);\n"
    "   gAlbedo.rgb = v4color.rgb;\n"
    "   gAlbedo.a *= v4color.a;\n"
    "}\n";

const std::string uiSpriteFragmentShader =
    "#version 410 core\n"
    "out vec4 gAlbedo;\n"
    "uniform float fOpacity;\n"
    "uniform sampler2D tTexture;\n"
    "in vec2 texCoord;\n"
    "void main() {\n"
    "   gAlbedo.rgba = texture(tTexture, texCoord);\n"
    "   gAlbedo.a *= fOpacity;\n"
    "}\n";
