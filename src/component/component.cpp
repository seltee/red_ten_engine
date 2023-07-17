// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/component.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "common/commonShaders.h"
#include "opengl/glew.h"
#include "stage/layerActors.h"
#include <math.h>

Component::Component()
{
}
Component::~Component()
{
}

void Component::prepare(Entity *owner)
{
    this->owner = owner;
}

void Component::process(float delta)
{
}

void Component::render(Matrix4 &vpMatrix, Transformation *tf)
{
    bool state = onRenderPrepare(vpMatrix, tf, false);
    if (state)
    {
        int drawAmount = getVertexAmount();
        if (drawAmount)
            glDrawArrays(drawAmount == 4 ? GL_QUADS : GL_TRIANGLES, 0, drawAmount);
    }
}

void Component::shadowRender(Matrix4 &vpMatrix, Transformation *tf)
{
    bool state = onRenderPrepare(vpMatrix, tf, true);
    if (state)
    {
        int drawAmount = getVertexAmount();
        if (drawAmount)
            glDrawArrays(GL_TRIANGLES, 0, drawAmount);
    }
}

void Component::renderLightPhase(Matrix4 &vpMatrix, unsigned int shadowMapTexture)
{
}

Matrix4 Component::preparePreShadowPhase(Vector3 cameraPosition)
{
    Matrix4 m;
    return m;
}

bool Component::onRenderPrepare(Matrix4 &vpMatrix, Transformation *tf, bool isShadowStage)
{
    return false;
}

void Component::prepareColorMode()
{
    switch (colorMode)
    {
    case ComponentColorMode::Lit:
        return glBlendFunc(GL_ZERO, GL_ONE);
    case ComponentColorMode::Alpha:
        return glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    case ComponentColorMode::Addition:
        return glBlendFunc(GL_ONE, GL_ONE);
    }
}

int Component::getVertexAmount()
{
    return 0;
}

ShapeSphere *Component::addShapeSphere(float radius)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeSphere(Vector3(0.0f), radius, world);
        shapes.push_back(newPhysicsEntity);
        if (owner)
            owner->childUpdated();
        return newPhysicsEntity;
    }

    return nullptr;
}

ShapeSphere *Component::addShapeSphere(Vector3 center, float radius)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeSphere(center, radius, world);
        shapes.push_back(newPhysicsEntity);
        if (owner)
            owner->childUpdated();
        return newPhysicsEntity;
    }
    return nullptr;
}

ShapeBox *Component::addShape2dBox(Vector2 size)
{
    return addShape2dBox(Vector2(0.0f), size);
}

ShapeBox *Component::addShape2dBox(Vector2 center, Vector2 size)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeBox(center, size, world);
        shapes.push_back(newPhysicsEntity);
        if (owner)
            owner->childUpdated();

        return newPhysicsEntity;
    }
    return nullptr;
}

ShapeBox *Component::addShape3dBox(Vector3 size)
{
    return addShape3dBox(Vector3(0.0f), size);
}

ShapeBox *Component::addShape3dBox(Vector3 center, Vector3 size)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeBox(center, size, world);
        shapes.push_back(newPhysicsEntity);
        if (owner)
            owner->childUpdated();
        return newPhysicsEntity;
    }
    return nullptr;
}

ShapeGeometry *Component::addShapeGeometry(Geometry *geometry)
{
    return addShapeGeometry(Vector3(0.0f), geometry);
}

ShapeGeometry *Component::addShapeGeometry(Vector3 center, Geometry *geometry)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeGeometry(center, geometry, world);
        shapes.push_back(newPhysicsEntity);
        if (owner)
            owner->childUpdated();
        return newPhysicsEntity;
    }
    return nullptr;
}

Matrix4 Component::getLocalspaceMatrix()
{
    return Matrix4(1.0f);
}
