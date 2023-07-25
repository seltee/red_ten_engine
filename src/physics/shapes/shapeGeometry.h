// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/geometry.h"
#include "physics/shapes/shape.h"
#include "math/math.h"
#include "connector/withDebug.h"

class PhysicsWorld;

class ShapeGeometry : public Shape,
                      public WithDebug
{
public:
    EXPORT ShapeGeometry(Vector3 center, Geometry *geometry, PhysicsWorld *world);
    EXPORT Geometry *getGeometry();

    EXPORT virtual ShapeCollisionType getType();

    EXPORT void provideTransformation(Matrix4 *transformation);

    EXPORT Vector3 getClosestPoint(Vector3 point);

    EXPORT bool testRay(Line line, std::vector<RayCollisionPoint> *points);

    EXPORT AABB getAABB();

    EXPORT void renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness);

protected:
    Geometry *geometry = nullptr;
    AABB aabb;

    Vector3 *vertexes;
    int vertexAmount;
    float simScale;
};
