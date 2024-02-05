// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentFramedSprite.h"
#include "math/glm/gtc/type_ptr.hpp"
#include <math.h>

ComponentFramedSprite::ComponentFramedSprite() : Component()
{
    mAnchor = Matrix4(1.0f);
    setAnchor(0.5f, 0.5f);
    shader = getRenderer()->getDefaultFramedSpriteShader();
    mesh = getRenderer()->getDefaultSpriteMesh();

    colorMode = ColorMode::Alpha;

    shadersParameters[0] = shader->createShaderParameter("aTexCoordShift", ShaderParameterType::Float2);
    shadersParameters[1] = shader->createShaderParameter("aTexCoordMul", ShaderParameterType::Float2);

    setShaderParameters(shadersParameters, 2);
}

ComponentFramedSprite::~ComponentFramedSprite()
{
    shader = getRenderer()->getDefaultFramedSpriteShader();
    shader->destroyShaderParameter(shadersParameters[0]);
    shader->destroyShaderParameter(shadersParameters[1]);
}

void ComponentFramedSprite::onRenderQueue(RenderQueue *renderQueue)
{
    if (texture && owner)
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix() * mAnchor;
        shadersParameters[0]->set(1, frameShift);
        shadersParameters[1]->set(1, frameRenderSize);

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

void ComponentFramedSprite::setOpacity(float opacity)
{
    this->opacity = fmaxf(fminf(opacity, 1.0f), 0.0f);
}

float ComponentFramedSprite::getOpacity()
{
    return opacity;
}

void ComponentFramedSprite::setAnchor(float x, float y)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-x, -y, 0.0f));
}

void ComponentFramedSprite::setAnchor(Vector2 &anchor)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-anchor.x, -anchor.y, 0.0f));
}

void ComponentFramedSprite::setRelativeScale(float x, float y)
{
    if (texture)
        transform.setScale((float)texture->getWidth() * frameSize[0] * x, (float)texture->getHeight() * frameSize[1] * y, 1.0f);
    else
        transform.setScale(1.0f, 1.0f, 1.0f);
}

void ComponentFramedSprite::setRelativeScale(Vector2 &size)
{
    if (texture)
        transform.setScale((float)texture->getWidth() * frameSize[0] * size.x, (float)texture->getHeight() * frameSize[1] * size.y, 1.0f);
    else
        transform.setScale(1.0f, 1.0f, 1.0f);
}

void ComponentFramedSprite::setTexture(Texture *texture)
{
    this->texture = texture;
    calcFrameRenderSize();
}

Texture *ComponentFramedSprite::getTexture()
{
    return texture;
}

void ComponentFramedSprite::setShader(Shader *shader)
{
    this->shader = shader;
}

void ComponentFramedSprite::setFramedRender(int framesInLine, int frameRows)
{
    if (framesInLine == 0)
        framesInLine = 1;
    if (frameRows == 0)
        frameRows = 1;

    this->framesInLine = framesInLine;
    framesTotal = framesInLine * frameRows;

    frameSize[0] = 1.0f / (float)framesInLine;
    frameSize[1] = 1.0f / (float)frameRows;

    setFrame(0);

    if (texture)
    {
        calcFrameRenderSize();
    }
}

void ComponentFramedSprite::setFrame(int frame)
{
    frame = frame % framesTotal;
    frameShift[0] = frameSize[0] * (frame % framesInLine);
    frameShift[1] = frameSize[1] * (frame / framesInLine);
    currentFrame = frame;
}

Matrix4 ComponentFramedSprite::getLocalspaceMatrix()
{
    Matrix4 mModelTransform = *transform.getModelMatrix();
    return mModelTransform * mAnchor;
}

void ComponentFramedSprite::calcFrameRenderSize()
{
    if (texture)
    {
        float pxSize = 1.0f / (float)texture->getWidth();
        float pySize = 1.0f / (float)texture->getHeight();
        frameRenderSize[0] = frameSize[0] - pxSize;
        frameRenderSize[1] = frameSize[1] - pySize;
    }
    else
    {
        frameRenderSize[0] = 1.0f;
        frameRenderSize[1] = 1.0f;
    }
    setRelativeScale(1.0f, 1.0f);
}