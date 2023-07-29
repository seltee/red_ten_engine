// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "physics/shapes/shape.h"
#include "math/math.h"

class PhysicsWorld;

class ShapePlain : public Shape
{
public:
    EXPORT ShapePlain(const Vector3 &normal, float distance, PhysicsWorld *world);
    EXPORT ShapeCollisionType getType();

    EXPORT inline Vector3 getClosestPoint(const Vector3 &point)
    {
        float t = glm::dot(normal, point) - distance;
        return point - t * normal;
    }

    EXPORT bool testRay(const Segment &line, std::vector<RayCollisionPoint> *points);

    EXPORT AABB getAABB();

    Vector3 normal;
    float distance;
};
