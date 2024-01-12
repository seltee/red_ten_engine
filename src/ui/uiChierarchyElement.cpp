// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/uiChierarchyElement.h"
#include "math/transformation.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/opengl/textureOpenGL.h"
#include "ui/uiNode.h"
#include "rtengine.h"
#include "renderer/opengl/glew.h"
#include <SDL_ttf.h>
#include <SDL.h>

std::vector<UITextCacheElement> UIChierarchyElement::textTexturesCache;

UIChierarchyElement::UIChierarchyElement()
{
    memset(&this->renderData, 0, sizeof(UIRenderData));
}

UIChierarchyElement::UIChierarchyElement(UINode *node, UIRenderData &renderData)
{
    this->node = node;
    this->renderData = renderData;
    this->renderData.text = renderData.text;
}

void UIChierarchyElement::render(RenderTarget *renderTarget)
{
    glScissor(
        renderData.viewport.x,
        renderTarget->getHeight() - renderData.viewport.y - renderData.viewport.endY,
        renderData.viewport.endX - renderData.viewport.x,
        renderData.viewport.endY - renderData.viewport.y);

    // Background
    if (renderData.background.a != 0.0f)
    {
        Transformation transform;
        transform.setPosition(Vector3(renderData.x, renderData.y, 0.0f));
        transform.setScale(Vector3(renderData.width, renderData.height, 1.0f));

        Matrix4 m = *transform.getModelMatrix();
        Matrix4 mModelProjection = *renderData.view * m;
        renderData.colorShader->use(m, mModelProjection);

        float color[4] = {renderData.background.r, renderData.background.g, renderData.background.b, renderData.background.a};

        renderData.colorShaderParameter->set(1, color);
        renderData.colorShaderParameter->apply();

        CommonOpenGLShaders::getSpriteMesh()->render();
    }

    // Image
    if (renderData.image)
    {
        TextureOpengGL *imageTexture = reinterpret_cast<TextureOpengGL *>(renderData.image);

        Transformation transform;
        transform.setPosition(Vector3(renderData.imageX, renderData.imageY, 0.0f));
        transform.setScale(Vector3(renderData.imageWidth, renderData.imageHeight, 1.0f));

        Matrix4 m = *transform.getModelMatrix();
        Matrix4 mModelProjection = *renderData.view * m;
        renderData.imageShader->use(m, mModelProjection);
        renderData.imageShader->setOpacity(renderData.imageAlpha);

        imageTexture->bind();

        CommonOpenGLShaders::getSpriteMesh()->render();

        renderData.colorShader->use(m, mModelProjection);
    }

    // Render Text
    if (renderData.text.length() > 0)
    {
        auto cachedText = getTextCacheElement(static_cast<int>(renderData.fontSize * renderData.zoom), renderData.text);
        if (cachedText)
        {
            TextureOpengGL *textTexture = reinterpret_cast<TextureOpengGL *>(cachedText->texture);
            float textWidth = cachedText->textureWidth / renderData.zoom;
            float textHeight = cachedText->textureHeight / renderData.zoom;

            Transformation transform;
            float shiftX = renderData.padding[0];
            float shiftY = renderData.padding[1];

            if (renderData.textHorizontalAlign == UITextAlign::Middle)
                shiftX += (renderData.width - textWidth - renderData.padding[0] - renderData.padding[2]) / 2.0f;
            if (renderData.textHorizontalAlign == UITextAlign::End)
                shiftX = renderData.width - textWidth - renderData.padding[2];
            if (renderData.textVerticalAlign == UITextAlign::Middle)
                shiftY += (renderData.height - textHeight - renderData.padding[1] - renderData.padding[3]) / 2.0f;
            if (renderData.textVerticalAlign == UITextAlign::End)
                shiftY = renderData.height - textHeight - renderData.padding[3];

            transform.setPosition(Vector3(renderData.x + shiftX, renderData.y + textHeight + shiftY, 0.0f));
            transform.setScale(Vector3(textWidth, -textHeight, 1.0f));

            Matrix4 m = *transform.getModelMatrix();
            Matrix4 mModelProjection = *renderData.view * m;

            renderData.textColorShader->use(m, mModelProjection);

            float color[4] = {renderData.textColor.r, renderData.textColor.g, renderData.textColor.b, renderData.textColor.a};
            renderData.colorTextShaderParameter->set(1, color);
            renderData.colorTextShaderParameter->apply();

            textTexture->bind();

            CommonOpenGLShaders::getSpriteMesh()->render();
        }
    }

    for (auto &child : children)
    {
        child.render(renderTarget);
    }
}

bool UIChierarchyElement::propagateScroll(float mouseX, float mouseY, float scroll)
{
    if (mouseX >= renderData.x && mouseX < renderData.x + renderData.width && mouseY >= renderData.y && mouseY < renderData.y + renderData.height)
    {
        bool result = false;
        for (auto child = children.rbegin(); child != children.rend(); ++child)
            result = result || child->propagateScroll(mouseX, mouseY, scroll);

        if (!result && renderData.scroll != UIScroll::None && node)
        {
            node->doScroll(scroll, max(renderData.contentDimensionsHeight - renderData.height + renderData.padding[1] + renderData.padding[3], 0));
        }
        return renderData.scroll != UIScroll::None;
    }
    return false;
}

bool UIChierarchyElement::pickPoint(float x, float y, std::vector<UIMousePickELement> *collection)
{
    if (x >= renderData.x && x < renderData.x + renderData.width && y >= renderData.y && y < renderData.y + renderData.height)
    {
        collection->push_back({node, renderData.x, renderData.y});

        for (auto child = children.rbegin(); child != children.rend(); ++child)
        {
            bool result = child->pickPoint(x, y, collection);
            if (result)
                break;
        }
        return true;
    }
    return false;
}

float UIChierarchyElement::getTextWidth(int fontSize, std::string text)
{
    if (text.length() > 0)
    {
        auto engine = RTEngine::getInstance();
        auto resourceController = engine->getResourceController();
        auto font = resourceController->addFont(fontSize);

        int w, h;
        TTF_SizeText((TTF_Font *)font->getFont(), text.c_str(), &w, &h);
        return static_cast<float>(w);
    }
    return 0.0f;
}

float UIChierarchyElement::getTextHeight(int fontSize, std::string text)
{
    if (text.length() > 0)
    {
        auto engine = RTEngine::getInstance();
        auto resourceController = engine->getResourceController();
        auto font = resourceController->addFont(fontSize);

        int w, h;
        TTF_SizeText((TTF_Font *)font->getFont(), text.c_str(), &w, &h);
        return static_cast<float>(h);
    }
    return 0.0f;
}

UITextCacheElement *UIChierarchyElement::getTextCacheElement(int fontSize, std::string text)
{
    for (auto &el : textTexturesCache)
    {
        if (el.fontSize == fontSize && el.text == text)
        {
            return &el;
        }
    }

    if (!getRenderer())
        return nullptr;

    auto engine = RTEngine::getInstance();
    auto resourceController = engine->getResourceController();
    auto font = resourceController->addFont(fontSize);

    SDL_Color color = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Blended((TTF_Font *)font->getFont(), text.c_str(), color);
    int textTextureWidth = surface->pitch / 4;
    int textTextureHeight = surface->h;
    Texture *textTexture = getRenderer()->createTexture(textTextureWidth, textTextureHeight, 4, surface->pixels, false);
    SDL_FreeSurface(surface);

    textTexturesCache.push_back({textTexture, text, fontSize, static_cast<float>(textTextureWidth), static_cast<float>(textTextureHeight)});
    return &textTexturesCache.back();
}