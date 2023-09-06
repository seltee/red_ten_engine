// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeBox.h"
#include "physics/physicsWorld.h"

ShapeBox::ShapeBox(Vector3 center, Vector3 size, PhysicsWorld *world) : ShapeConvex(center, world)
{
    this->worldSize = size;
    this->size = size * simScale;

    mass = this->size.x * this->size.y * this->size.z * 20.0f;
    float width2 = this->size.x * this->size.x;
    float height2 = this->size.y * this->size.y;
    float depth2 = this->size.z * this->size.z;

    inertia[0] = Vector3((1.0f / 12.0f) * mass * (height2 + depth2), 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, (1.0f / 12.0f) * mass * (width2 + depth2), 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, (1.0f / 12.0f) * mass * (width2 + height2));
}

ShapeBox::ShapeBox(Vector2 center, Vector2 size, PhysicsWorld *world) : ShapeConvex(Vector3(center.x, center.y, 0.0f), world)
{
    Vector3 size3 = Vector3(size.x, size.y, (size.x + size.y) / 2.0f);

    this->worldSize = size3;
    this->size = size3 * simScale;
    mass = this->size.x * this->size.y * this->size.z * 16.0f;

    float width2 = this->size.x * this->size.x;
    float height2 = this->size.y * this->size.y;
    float depth2 = this->size.z * this->size.z;

    inertia[0] = Vector3((1.0f / 12.0f) * mass * (height2 + depth2), 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, (1.0f / 12.0f) * mass * (width2 + depth2), 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, (1.0f / 12.0f) * mass * (width2 + height2));
}

ShapeCollisionType ShapeBox::getType()
{
    return ShapeCollisionType::OBB;
}

Matrix3 ShapeBox::getInertiaTensor()
{
    return inertia;
}

Vector3 ShapeBox::getClosestPoint(Matrix4 OBBTransformation, Vector3 point)
{
    Matrix4 invTransformation = glm::inverse(OBBTransformation);
    Vector4 point4 = Vector4(point.x, point.y, point.z, 1.0f);
    Vector3 localPoint = Vector3(invTransformation * point4);

    Vector3 min = -size / 2.0f;
    Vector3 max = size / 2.0f;

    Vector3 result;

    result.x = localPoint.x;
    if (result.x < min.x)
        result.x = min.x;
    if (result.x > max.x)
        result.x = max.x;

    result.y = localPoint.y;
    if (result.y < min.y)
        result.y = min.y;
    if (result.y > max.y)
        result.y = max.y;

    result.z = localPoint.z;
    if (result.z < min.z)
        result.z = min.z;
    if (result.z > max.z)
        result.z = max.z;

    return Vector3(OBBTransformation * Vector4(result.x, result.y, result.z, 1.0f));
}

Hull *ShapeBox::getHull()
{
    if (!hull)
    {
        Vector3 halfSize = worldSize / 2.0f;
        Vector3 verticies[8] = {
            -halfSize,
            halfSize,
            Vector3(halfSize.x, -halfSize.y, -halfSize.z),
            Vector3(-halfSize.x, -halfSize.y, halfSize.z),
            Vector3(halfSize.x, -halfSize.y, halfSize.z),
            Vector3(-halfSize.x, halfSize.y, -halfSize.z),
            Vector3(halfSize.x, halfSize.y, -halfSize.z),
            Vector3(-halfSize.x, halfSize.y, halfSize.z),
        };

        hull = setNewHull(verticies, 8);

        int p1[] = {0, 2, 4, 3};
        hull->addPolygon(p1, 4);

        int p2[] = {1, 6, 5, 7};
        hull->addPolygon(p2, 4);

        int p3[] = {0, 5, 6, 2};
        hull->addPolygon(p3, 4);

        int p4[] = {4, 1, 7, 3};
        hull->addPolygon(p4, 4);

        int p5[] = {0, 3, 7, 5};
        hull->addPolygon(p5, 4);

        int p6[] = {2, 6, 1, 4};
        hull->addPolygon(p6, 4);

        hull->rebuildEdges();
    }
    return hull;
}