// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/geometry.h"
#include "physics/shapes/shape.h"
#include "math/math.h"

class PhysicsWorld;

class ShapeGeometry : public Shape
{
public:
    EXPORT ShapeGeometry(Vector3 center, Geometry *geometry, PhysicsWorld *world);

    EXPORT Geometry *getGeometry();

protected:
    Geometry *geometry = nullptr;
};
