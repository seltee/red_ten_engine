// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shape.h"

Shape::Shape(Vector3 center)
{
    this->center = center;
}

ShapeCollisionType Shape::getType()
{
    return ShapeCollisionType::None;
}

Matrix3 Shape::getInertiaTensor()
{
    Matrix3 inertia;
    inertia[0] = Vector3(1.0f, 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, 1.0f, 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, 1.0f);
    return inertia;
}

std::string Shape::getTypeName(ShapeCollisionType type)
{
    switch (type)
    {
    case ShapeCollisionType::None:
        return "None";
    case ShapeCollisionType::Sphere:
        return "Sphere";
    case ShapeCollisionType::OBB:
        return "OBB";
    case ShapeCollisionType::Convex:
        return "Convex";
    case ShapeCollisionType::Plain:
        return "Plain";
    case ShapeCollisionType::Polygon:
        return "PolygonalGeometry";
    case ShapeCollisionType::Amount:
        return "Amount???";
    }
    return "Unknown";
}

void Shape::provideTransformation(Matrix4 *transformation)
{
}

bool Shape::testRay(Line line, std::vector<RayCollisionPoint> *points)
{
    return false;
}

void Shape::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness) {}

AABB Shape::getAABB()
{
    return AABB(Vector3(0.0f), Vector3(0.0f));
}