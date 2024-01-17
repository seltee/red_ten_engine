// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "ui/uiRenderElement.h"
#include "math/transformation.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/opengl/textureOpenGL.h"
#include "ui/uiNode.h"
#include "rtengine.h"
#include "renderer/opengl/glew.h"
#include <SDL_ttf.h>
#include <SDL.h>

std::vector<UITextCacheElement> UIRenderElement::textTexturesCache;

UIRenderElement::UIRenderElement()
{
}

UIRenderElement::UIRenderElement(UINode *node, UIRenderData *renderData)
{
    this->node = node;
    this->renderData = renderData;
}

void UIRenderElement::render(RenderTarget *renderTarget, UIRenderSharedData *sharedData)
{
    glScissor(
        renderData->viewport.x,
        renderTarget->getHeight() - renderData->viewport.endY,
        renderData->viewport.endX - renderData->viewport.x,
        renderData->viewport.endY - renderData->viewport.y);

    // Background
    if (renderData->background.a != 0.0f)
    {
        Transformation transform;
        transform.setPosition(Vector3(renderData->x, renderData->y, 0.0f));
        transform.setScale(Vector3(renderData->width, renderData->height, 1.0f));

        Matrix4 m = *transform.getModelMatrix();
        Matrix4 mModelProjection = *sharedData->view * m;
        sharedData->colorShader->use(m, mModelProjection);

        float color[4] = {renderData->background.r, renderData->background.g, renderData->background.b, renderData->background.a};

        sharedData->colorShaderParameter->set(1, color);
        sharedData->colorShaderParameter->apply();

        CommonOpenGLShaders::getSpriteMesh()->render();
    }

    // Image
    if (renderData->image)
    {
        TextureOpengGL *imageTexture = reinterpret_cast<TextureOpengGL *>(renderData->image);

        Transformation transform;
        transform.setPosition(Vector3(renderData->imageX, renderData->imageY + renderData->imageHeight, 0.0f));
        transform.setScale(Vector3(renderData->imageWidth, -renderData->imageHeight, 1.0f));

        Matrix4 m = *transform.getModelMatrix();
        Matrix4 mModelProjection = *sharedData->view * m;
        sharedData->imageShader->use(m, mModelProjection);
        sharedData->imageShader->setOpacity(renderData->imageAlpha);
        sharedData->imageShiftShaderParameter->set(1, renderData->imageShift);
        sharedData->imageShiftShaderParameter->apply();
        sharedData->imageFrameShaderParameter->set(1, renderData->imageFrame);
        sharedData->imageFrameShaderParameter->apply();

        imageTexture->bind();

        CommonOpenGLShaders::getSpriteMesh()->render();

        sharedData->colorShader->use(m, mModelProjection);
    }

    // Render Text
    if (renderData->text.length() > 0)
    {
        auto cachedText = getTextCacheElement(static_cast<int>(renderData->fontSize * sharedData->zoom), renderData->text);
        if (cachedText)
        {
            TextureOpengGL *textTexture = reinterpret_cast<TextureOpengGL *>(cachedText->texture);
            float textWidth = cachedText->textureWidth / sharedData->zoom;
            float textHeight = cachedText->textureHeight / sharedData->zoom;

            Transformation transform;
            float shiftX = renderData->padding[0];
            float shiftY = renderData->padding[1];

            if (renderData->textHorizontalAlign == UITextAlign::Middle)
                shiftX += (renderData->width - textWidth - renderData->padding[0] - renderData->padding[2]) / 2.0f;
            if (renderData->textHorizontalAlign == UITextAlign::End)
                shiftX = renderData->width - textWidth - renderData->padding[2];
            if (renderData->textVerticalAlign == UITextAlign::Middle)
                shiftY += (renderData->height - textHeight - renderData->padding[1] - renderData->padding[3]) / 2.0f;
            if (renderData->textVerticalAlign == UITextAlign::End)
                shiftY = renderData->height - textHeight - renderData->padding[3];

            transform.setPosition(Vector3(renderData->x + shiftX, renderData->y + textHeight + shiftY, 0.0f));
            transform.setScale(Vector3(textWidth, -textHeight, 1.0f));

            Matrix4 m = *transform.getModelMatrix();
            Matrix4 mModelProjection = *sharedData->view * m;

            sharedData->textColorShader->use(m, mModelProjection);

            float color[4] = {renderData->textColor.r, renderData->textColor.g, renderData->textColor.b, renderData->textColor.a};
            sharedData->colorTextShaderParameter->set(1, color);
            sharedData->colorTextShaderParameter->apply();

            textTexture->bind();

            CommonOpenGLShaders::getSpriteMesh()->render();
        }
    }
}

float UIRenderElement::getTextWidth(int fontSize, std::string text)
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

float UIRenderElement::getTextHeight(int fontSize, std::string text)
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

UITextCacheElement *UIRenderElement::getTextCacheElement(int fontSize, std::string text)
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