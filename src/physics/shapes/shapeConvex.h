// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "math/math.h"
#include "shape.h"
#include "physics/hull.h"
#include "connector/withDebug.h"
#include "physics/physicsWorld.h"
#include <string>

struct FaceQuery
{
    float separation;
    HullPolygon *polygon;
    Vector3 axis;
};

struct EdgeQuery
{
    float separation;
    HullEdge *edgeA;
    HullEdge *edgeB;
    Vector3 axis;
};

class ShapeConvex : public Shape,
                    public WithDebug
{
public:
    ShapeConvex(Vector3 center, PhysicsWorld *world) : Shape(center)
    {
        simScale = world ? world->getSimScale() : 1.0f;
        mass = 1.0f;
    }
    ~ShapeConvex();

    EXPORT virtual Hull *getHull() { return hull; }

    EXPORT Hull *setNewHull(Vector3 *verticies, int amount);

    EXPORT void calcMassByHull(float density = 0.1f);

    EXPORT void provideTransformation(Matrix4 *transformation);
    EXPORT void updateTransformation();

    EXPORT virtual ShapeCollisionType getType();

    EXPORT bool testRay(const Segment &line, std::vector<RayCollisionPoint> *points);

    EXPORT AABB getAABB();

    EXPORT FaceQuery queryFaceDirection(ShapeConvex *foreignShape);
    EXPORT EdgeQuery queryEdgeDirection(ShapeConvex *foreignShape);

    EXPORT Vector3 findFurthestPoint(const Vector3 &inDirection);
    EXPORT Vector3 getClosestPointToHull(const Vector3 &point);

    EXPORT bool checkHullConvexity();

    EXPORT void renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness);
    EXPORT inline Vector3 getHullCenter() { return getHull()->hullCenter; }

protected:
    Hull *hull = nullptr;
    Matrix4 transformation;
    bool isDirty = true;
    AABB aabb;
    float simScale = 0.0f;
};
