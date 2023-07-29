// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "math/math.h"
#include <string>
#include <vector>

struct RayCollisionPoint
{
    Vector3 point;
    float distance;
};

struct CollisionPoint
{
    Vector3 point;
    Vector3 normal;
    float depth;
    bool hasCollision;
};

enum class ShapeCollisionType
{
    None = 0,
    Plain = 1,
    Sphere = 2,
    Convex = 3,
    OBB = 4,
    Geometry = 5,
    Capsule = 6,
    Amount = 7
};

class Shape
{
protected:
    Shape(Vector3 center);

public:
    virtual ~Shape();
    EXPORT virtual ShapeCollisionType getType();
    EXPORT virtual Matrix3 getInertiaTensor();
    EXPORT inline float getMass() { return mass; }
    EXPORT static std::string getTypeName(ShapeCollisionType type);
    EXPORT virtual void provideTransformation(Matrix4 *transformation);

    EXPORT virtual bool testRay(const Segment &line, std::vector<RayCollisionPoint> *points);

    EXPORT virtual void renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness);

    EXPORT virtual AABB getAABB();

    EXPORT inline void setDebugName(std::string str) { name = str; }

    static Vector3 debugColorWireframe;

protected:
    Vector3 center;
    float mass;
    std::string name;
};
