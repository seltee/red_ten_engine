// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "physics/shapes/shape.h"
#include "math/math.h"
#include "connector/withDebug.h"

class PhysicsWorld;

class ShapeSphere : public Shape,
                    public WithDebug
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

    EXPORT void renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness);

protected:
    float radius;
    Matrix3 inertia;
    Vector3 absoluteCenter;
};
