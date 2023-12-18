// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentSprite.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "actor/actor.h"
#include <math.h>

ComponentSprite::ComponentSprite() : Component()
{
    mAnchor = Matrix4(1.0f);
    setAnchor(0.5f, 0.5f);
    colorMode = ComponentColorMode::Alpha;
    shader = getRenderer()->getDefaultSpriteShader();
    mesh = getRenderer()->getDefaultSpriteMesh();
}

void ComponentSprite::onRenderQueue(RenderQueue *renderQueue)
{
    if (texture && owner)
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix() * mAnchor;
        if (colorMode == ComponentColorMode::Lit)
        {
            renderQueue->addMainPhase(mModel, shader, texture, mesh, parametersList, parametersAmount);
        }
        else
        {
            renderQueue->addBlendingPhase(mModel, colorMode, shader, texture, mesh, opacity, parametersList, parametersAmount);
        }
    }
}

void ComponentSprite::setOpacity(float opacity)
{
    this->opacity = fmaxf(fminf(opacity, 1.0f), 0.0f);
}

float ComponentSprite::getOpacity()
{
    return opacity;
}

void ComponentSprite::setAnchor(float x, float y)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-x, -y, 0.0f));
}

void ComponentSprite::setAnchor(Vector2 &anchor)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-anchor.x, -anchor.y, 0.0f));
}

void ComponentSprite::setRelativeScale(float x, float y)
{
    if (texture)
        transform.setScale((float)texture->getWidth() * x, (float)texture->getHeight() * y, 1.0f);
    else
        transform.setScale(1.0f, 1.0f, 1.0f);
}

void ComponentSprite::setRelativeScale(Vector2 &size)
{
    if (texture)
        transform.setScale((float)texture->getWidth() * size.x, (float)texture->getHeight() * size.y, 1.0f);
    else
        transform.setScale(1.0f, 1.0f, 1.0f);
}

void ComponentSprite::setTexture(Texture *texture)
{
    this->texture = texture;
    setRelativeScale(1.0f, 1.0f);
}

Texture *ComponentSprite::getTexture()
{
    return texture;
}

void ComponentSprite::setShader(Shader *shader)
{
    this->shader = shader;
}

Matrix4 ComponentSprite::getLocalspaceMatrix()
{
    Matrix4 mModelTransform = *transform.getModelMatrix();
    return mModelTransform * mAnchor;
}
