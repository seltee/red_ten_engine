// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeGeometry.h"

ShapeGeometry::ShapeGeometry(Vector3 center, Geometry *geometry, PhysicsWorld *world) : Shape(center)
{
    this->geometry = geometry;
}

Geometry *ShapeGeometry::getGeometry()
{
    return geometry;
}
