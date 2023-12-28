// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeSphere.h"
#include "physics/physicsWorld.h"

ShapeSphere::ShapeSphere(const Vector3 &center, float radius, PhysicsWorld *world) : Shape(center)
{
    this->radius = radius * world->getSimScale();

    mass = (4.0f / 3.0f) * CONST_PI * this->radius * this->radius * this->radius * 25.0f;

    float tenValue = 2.0f / 5.0f * mass * this->radius * this->radius;
    inertia[0] = Vector3(tenValue, 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, tenValue, 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, tenValue);
}

float ShapeSphere::getRadius()
{
    return radius;
}

ShapeCollisionType ShapeSphere::getType()
{
    return ShapeCollisionType::Sphere;
}

Matrix3 ShapeSphere::getInertiaTensor()
{
    return inertia;
}

Vector3 ShapeSphere::getAbsoluteCenter()
{
    return absoluteCenter;
}

bool ShapeSphere::testRay(const Segment &line, std::vector<RayCollisionPoint> *points)
{
    Vector3 normal = glm::normalize(line.b - line.a);
    Vector3 m = line.a - absoluteCenter;
    float b = glm::dot(m, normal);
    float c = glm::dot(m, m) - radius * radius;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return false;
    float discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return false;

    // Ray now found to intersect sphere, compute both points of intersection
    float sqrtDiscr = sqrtf(discr);
    float t1 = -b - sqrtDiscr;
    float t2 = -b + sqrtDiscr;
    float length = glm::length(line.b - line.a);

    // If t is negative, ray started inside sphere
    if (t1 < 0.0f)
    {
        if (t2 <= length)
        {
            Vector3 point = line.a + t2 * normal;
            points->push_back({point, glm::normalize(absoluteCenter - point), t2});
        }
    }
    else
    {
        if (t1 <= length)
        {
            Vector3 point = line.a + t1 * normal;
            points->push_back({point, glm::normalize(absoluteCenter - point), t1});
        }
        if (t2 <= length)
        {
            Vector3 point = line.a + t2 * normal;
            points->push_back({point, glm::normalize(absoluteCenter - point), t2});
        }
    }

    return true;
}

void ShapeSphere::provideTransformation(Matrix4 *transformation)
{
    absoluteCenter = Vector3(*transformation * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

AABB ShapeSphere::getAABB()
{
    return AABB(absoluteCenter - Vector3(radius, radius, radius), absoluteCenter + Vector3(radius, radius, radius));
}

void ShapeSphere::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness)
{

    for (int i = 0; i < 16; i++)
    {
        float lA = (float)i / 16.0f * CONST_PI * 2.0f;
        float lB = (float)(i + 1) / 16.0f * CONST_PI * 2.0f;

        debug->renderLine(
            (absoluteCenter + Vector3(radius * sinf(lA), radius * cosf(lA), 0.0f)) * scale,
            (absoluteCenter + Vector3(radius * sinf(lB), radius * cosf(lB), 0.0f)) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCenter + Vector3(0.0f, radius * sinf(lA), radius * cosf(lA))) * scale,
            (absoluteCenter + Vector3(0.0f, radius * sinf(lB), radius * cosf(lB))) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCenter + Vector3(radius * cosf(lA), 0.0f, radius * sinf(lA))) * scale,
            (absoluteCenter + Vector3(radius * cosf(lB), 0.0f, radius * sinf(lB))) * scale,
            projectionView, thickness, debugColorWireframe);
    }
}