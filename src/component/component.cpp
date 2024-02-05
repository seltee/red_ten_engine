// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/component.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "stage/layerActors.h"
#include "actor/actor.h"
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

void Component::onRenderQueue(RenderQueue *renderQueue)
{
}

void Component::onCreated()
{
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

Matrix4 Component::getWorldModelMatrix()
{
    if (owner)
    {
        return *owner->transform.getModelMatrix() * *transform.getModelMatrix();
    }
    return Matrix4(1.0f);
}

MeshStatic *Component::getStaticMesh()
{
    return nullptr;
}

void Component::renderDebugVolume(Renderer *renderer, Matrix4 *mProjectionView, float thickness, Vector3 color)
{
}

void Component::setShaderParameters(ShaderParameter **parametersList, int parametersAmount)
{
    this->parametersList = parametersList;
    this->parametersAmount = parametersAmount;
}