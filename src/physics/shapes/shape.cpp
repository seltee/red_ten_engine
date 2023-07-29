// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shape.h"

Vector3 Shape::debugColorWireframe = Vector3(0.4f, 0.4f, 0.4f);

Shape::Shape(Vector3 center)
{
    this->center = center;
}

Shape::~Shape()
{
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
    case ShapeCollisionType::Geometry:
        return "PolygonalGeometry";
    case ShapeCollisionType::Capsule:
        return "Capsule";
    case ShapeCollisionType::Amount:
        return "Amount???";
    }
    return "Unknown";
}

void Shape::provideTransformation(Matrix4 *transformation)
{
}

bool Shape::testRay(const Segment &line, std::vector<RayCollisionPoint> *points)
{
    return false;
}

void Shape::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness) {}

AABB Shape::getAABB()
{
    return AABB(Vector3(0.0f), Vector3(0.0f));
}