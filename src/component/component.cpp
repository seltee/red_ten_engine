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
    onCreated();
}

void Component::process(float delta)
{
}

void Component::onRender(Matrix4 &vpMatrix, Transformation *tf)
{
}

void Component::onRenderShadow(Matrix4 &vpMatrix, Transformation *tf)
{
}

void Component::renderLightPhase(Matrix4 &vpMatrix, unsigned int shadowMapTexture)
{
}

Matrix4 Component::preparePreShadowPhase(Vector3 cameraPosition)
{
    Matrix4 m;
    return m;
}

void Component::onCreated()
{
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

ShapeConvex *Component::addShapeConvex(Vector3 *verticies, int amount, std::vector<HullPolygonSimple> *polygons, float density)
{
    return addShapeConvex(Vector3(0.0f), verticies, amount, polygons, density);
}

ShapeConvex *Component::addShapeConvex(Vector3 center, Vector3 *verticies, int amount, std::vector<HullPolygonSimple> *polygons, float density)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeConvex(center, world);
        auto hull = newPhysicsEntity->setNewHull(verticies, amount);
        if (polygons)
        {
            hull->addPolygons(polygons);
            hull->rebuildEdges();
            newPhysicsEntity->calcMassByHull(density);
        }
        else
            newPhysicsEntity->setMass(1.0f);

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

ShapeCapsule *Component::addShapeCapsule(float height, float radius)
{
    auto world = owner->getCurrentLayer()->getPhysicsWorld();
    if (world)
    {
        auto newPhysicsEntity = new ShapeCapsule(height, radius, world);
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

MeshStatic *Component::getStaticMesh()
{
    return nullptr;
}