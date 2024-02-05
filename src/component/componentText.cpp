// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "math/glm/gtc/type_ptr.hpp"
#include "component/componentText.h"
#include <math.h>
#include <SDL_ttf.h>
#include <SDL.h>

ComponentText::ComponentText()
{
    mAnchor = Matrix4(1.0f);
    setAnchor(0.5f, 0.5f);
    colorMode = ColorMode::Alpha;
    shader = getRenderer()->getDefaultSpriteShader();
    mesh = getRenderer()->getDefaultSpriteMesh();
}

ComponentText::~ComponentText()
{
    if (texture)
        delete texture;
}

void ComponentText::process(float delta)
{
    if (isStringDirty)
        rebuildString();
}

void ComponentText::onRenderQueue(RenderQueue *renderQueue)
{
    if (owner && texture)
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix() * mAnchor;
        if (colorMode == ColorMode::Lit)
        {
            renderQueue->addMainPhase(mModel, shader, texture, mesh, parametersList, parametersAmount);
        }
        else
        {
            renderQueue->addBlendingPhase(mModel, colorMode, shader, texture, mesh, opacity, parametersList, parametersAmount);
        }
    }
}

void ComponentText::setOpacity(float opacity)
{
    this->opacity = fmaxf(fminf(opacity, 1.0f), 0.0f);
}

float ComponentText::getOpacity()
{
    return opacity;
}

void ComponentText::setAnchor(float x, float y)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-x, -y, 0.0f));
}

void ComponentText::setAnchor(Vector2 &anchor)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-anchor.x, -anchor.y, 0.0f));
}

void ComponentText::setFont(ResourceFont *font)
{
    if (this->font != font)
    {
        this->font = font;
        isStringDirty = true;
    }
}

void ComponentText::setColor(unsigned char r, unsigned char g, unsigned char b)
{
    if (this->r != r || this->g != g || this->b != b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        isStringDirty = true;
    }
}

void ComponentText::setText(std::string string)
{
    if (this->string != string)
    {
        this->string = string;
        isStringDirty = true;
    }
}

std::string ComponentText::getText()
{
    return string;
}

void ComponentText::rebuildString()
{
    isStringDirty = false;
    if (texture)
        delete texture;

    if (font && font->getFont() && string.length() > 0)
    {
        SDL_Color color = {b, g, r};
        SDL_Surface *surface = TTF_RenderUTF8_Blended((TTF_Font *)font->getFont(), string.c_str(), color);

        if (!surface)
        {
            logger->logff("Failed to render text into surface: %s", string.c_str());
            textTextureWidth = 0;
            textTextureHeight = 0;
        }
        else
        {
            textTextureWidth = surface->pitch / 4;
            textTextureHeight = surface->h;
            transform.setScale(textTextureWidth, textTextureHeight);
            texture = getRenderer()->createTexture(textTextureWidth, textTextureHeight, 4, surface->pixels, false);
            SDL_FreeSurface(surface);
        }
    }
}

void ComponentText::setShader(Shader *shader)
{
    this->shader = shader;
}

int ComponentText::getWidth()
{
    return textTextureWidth;
}

int ComponentText::getHeight()
{
    return textTextureHeight;
}

Matrix4 ComponentText::getLocalspaceMatrix()
{
    Matrix4 mTransform = *transform.getModelMatrix();
    Matrix4 mOut(1.0f);
    mOut *= mAnchor;
    mOut *= mTransform;

    return mOut;
}