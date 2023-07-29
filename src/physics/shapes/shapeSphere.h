// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "physics/shapes/shape.h"
#include "math/math.h"

class PhysicsWorld;

class ShapeSphere : public Shape
{
public:
    EXPORT ShapeSphere(const Vector3 &center, float radius, PhysicsWorld *world);
    EXPORT float getRadius();

    EXPORT ShapeCollisionType getType();
    EXPORT Matrix3 getInertiaTensor();
    EXPORT Vector3 getAbsoluteCenter();

    EXPORT bool testRay(const Segment &line, std::vector<RayCollisionPoint> *points);

    EXPORT void provideTransformation(Matrix4 *transformation);
    EXPORT AABB getAABB();

protected:
    float radius;
    Matrix3 inertia;
    Vector3 absoluteCenter;
};
