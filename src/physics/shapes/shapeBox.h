// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "shape.h"
#include "shapeConvex.h"
#include "math/math.h"

class PhysicsWorld;

class ShapeBox : public ShapeConvex
{
public:
    EXPORT ShapeBox(Vector3 center, Vector3 size, PhysicsWorld *world);
    EXPORT ShapeBox(Vector2 center, Vector2 size, PhysicsWorld *world);

    EXPORT ShapeCollisionType getType();
    EXPORT Matrix3 getInertiaTensor();
    EXPORT Vector3 getClosestPoint(Matrix4 OBBTransformation, Vector3 point);

    EXPORT Hull *getHull();

    EXPORT inline Vector3 getSize() { return size; }

protected:
    Vector3 size;
    Vector3 worldSize;
    Matrix3 inertia;
};
