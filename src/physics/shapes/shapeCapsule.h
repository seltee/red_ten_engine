// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shape.h"
#include "shapeConvex.h"

class PhysicsWorld;

class ShapeCapsule : public Shape,
                     public WithDebug
{
public:
    EXPORT ShapeCapsule(float height, float radius, PhysicsWorld *world);
    EXPORT ~ShapeCapsule();
    EXPORT ShapeCollisionType getType();
    EXPORT Matrix3 getInertiaTensor();
    EXPORT Vector3 getClosestPoint(const Vector3 &point);

    EXPORT bool testRay(const Segment &line, std::vector<RayCollisionPoint> *points);

    EXPORT void provideTransformation(Matrix4 *transformation);
    EXPORT AABB getAABB();

    EXPORT void renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness);

    EXPORT inline float getRadius() { return radius; }
    EXPORT inline Segment getAbsoluteCapsule() { return absoluteCapsule; }

    EXPORT inline ShapeConvex *getAsConvex() { return convex; }

protected:
    Segment absoluteCapsule;
    float radius;
    float height;
    Matrix3 inertia;
    AABB aabb;
    ShapeConvex *convex;
};
