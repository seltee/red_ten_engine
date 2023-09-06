// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeCapsule.h"
#include "physics/physicsWorld.h"

ShapeCapsule::ShapeCapsule(float height, float radius, PhysicsWorld *world) : Shape(Vector3(0.0f, 0.0f, 0.0f))
{
    float simScale = world->getSimScale();
    this->radius = radius * simScale;
    this->height = height * simScale;
    absoluteCapsule.a = Vector3(0.0f, -0.5f * this->height, 0.0f);
    absoluteCapsule.b = Vector3(0.0f, 0.5f * this->height, 0.0f);

    float dRadius = this->radius * this->radius;
    float dHeight = this->height * this->height;
    float density = 20.0f;
    float massHalfSphere = (2.0f * dRadius * this->radius * CONST_PI) / 3.0f * density;
    float massCilinder = this->height * dRadius * CONST_PI * density;
    mass = massHalfSphere * 2.0f + massCilinder;

    float xz = massCilinder * (dHeight / 12.0f + dRadius / 4.0f) + 2.0f * massHalfSphere * ((2.0f * dRadius) / 5.0f + dHeight / 2.0f + (3.0f * this->height * this->radius) / 8.0f);
    float y = massCilinder * (dRadius / 2.0f) + 2.0f * massHalfSphere * ((2.0f * dRadius) / 5.0f);

    inertia[0] = Vector3(xz, 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, y, 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, xz);

    Vector3 points[2] = {absoluteCapsule.a, absoluteCapsule.b};
    convex = new ShapeConvex(Vector3(0.0f), world);
    convex->setNewHull(points, 2);
}

ShapeCapsule::~ShapeCapsule()
{
}

ShapeCollisionType ShapeCapsule::getType()
{
    return ShapeCollisionType::Capsule;
}

Matrix3 ShapeCapsule::getInertiaTensor()
{
    return inertia;
}

Vector3 ShapeCapsule::getClosestPoint(const Vector3 &point)
{
    return absoluteCapsule.getClosestPoint(point);
}

bool ShapeCapsule::testRay(const Segment &line, std::vector<RayCollisionPoint> *points)
{
    float s, t;
    Vector3 pOnLine, pOnCapsule;
    getClosestPointSegmentSegment(line.a, line.b, absoluteCapsule.a, absoluteCapsule.b, s, t, pOnLine, pOnCapsule);

    Vector3 normal = glm::normalize(line.b - line.a);
    Vector3 m = line.a - pOnCapsule;
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
            points->push_back({line.a + t2 * normal, t2});
    }
    else
    {
        if (t1 <= length)
            points->push_back({line.a + t1 * normal, t1});
        if (t2 <= length)
            points->push_back({line.a + t2 * normal, t2});
    }

    return true;
}

void ShapeCapsule::provideTransformation(Matrix4 *transformation)
{
    absoluteCapsule.a = *transformation * Vector4(0.0f, -0.5f * this->height, 0.0f, 1.0f);
    absoluteCapsule.b = *transformation * Vector4(0.0f, 0.5f * this->height, 0.0f, 1.0f);
    convex->getHull()->absoluteVerticies[0] = absoluteCapsule.a;
    convex->getHull()->absoluteVerticies[1] = absoluteCapsule.b;

    aabb.start.x = fminf(absoluteCapsule.a.x - radius, absoluteCapsule.b.x - radius);
    aabb.start.y = fminf(absoluteCapsule.a.y - radius, absoluteCapsule.b.y - radius);
    aabb.start.z = fminf(absoluteCapsule.a.z - radius, absoluteCapsule.b.z - radius);
    aabb.end.x = fmaxf(absoluteCapsule.a.x + radius, absoluteCapsule.b.x + radius);
    aabb.end.y = fmaxf(absoluteCapsule.a.y + radius, absoluteCapsule.b.y + radius);
    aabb.end.z = fmaxf(absoluteCapsule.a.z + radius, absoluteCapsule.b.z + radius);
}

AABB ShapeCapsule::getAABB()
{
    return aabb;
}

void ShapeCapsule::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness)
{
    Vector3 vDirection = absoluteCapsule.getDirection();
    Vector3 vSide = glm::cross(vDirection, Vector3(0.0f, 1.0f, 0.0f));
    Vector3 vPerpendicular = glm::cross(vSide, vDirection);
    Vector3 vHeight = absoluteCapsule.b - absoluteCapsule.a;

    debug->renderLine((absoluteCapsule.a + vSide * radius) * scale, (absoluteCapsule.a + vSide * radius + vHeight) * scale, projectionView, thickness, debugColorWireframe);
    debug->renderLine((absoluteCapsule.a - vSide * radius) * scale, (absoluteCapsule.a - vSide * radius + vHeight) * scale, projectionView, thickness, debugColorWireframe);
    debug->renderLine((absoluteCapsule.a + vPerpendicular * radius) * scale, (absoluteCapsule.a + vPerpendicular * radius + vHeight) * scale, projectionView, thickness, debugColorWireframe);
    debug->renderLine((absoluteCapsule.a - vPerpendicular * radius) * scale, (absoluteCapsule.a - vPerpendicular * radius + vHeight) * scale, projectionView, thickness, debugColorWireframe);

    for (int i = 0; i < 10; i++)
    {
        float lA = (float)i / 10.0f * CONST_PI * 2.0f;
        float lB = (float)(i + 1) / 10.0f * CONST_PI * 2.0f;

        debug->renderLine(
            (absoluteCapsule.a + vPerpendicular * radius * sinf(lA) + vSide * radius * cosf(lA)) * scale,
            (absoluteCapsule.a + vPerpendicular * radius * sinf(lB) + vSide * radius * cosf(lB)) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCapsule.b + vPerpendicular * radius * sinf(lA) + vSide * radius * cosf(lA)) * scale,
            (absoluteCapsule.b + vPerpendicular * radius * sinf(lB) + vSide * radius * cosf(lB)) * scale,
            projectionView, thickness, debugColorWireframe);
    }

    for (int i = 0; i < 6; i++)
    {
        float lA = (float)i / 6.0f * CONST_PI;
        float lB = (float)(i + 1) / 6.0f * CONST_PI;

        debug->renderLine(
            (absoluteCapsule.a + vPerpendicular * radius * cosf(lA) + vDirection * radius * sinf(lA)) * scale,
            (absoluteCapsule.a + vPerpendicular * radius * cosf(lB) + vDirection * radius * sinf(lB)) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCapsule.a + vSide * radius * cosf(lA) + vDirection * radius * sinf(lA)) * scale,
            (absoluteCapsule.a + vSide * radius * cosf(lB) + vDirection * radius * sinf(lB)) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCapsule.b + vPerpendicular * radius * cosf(lA) - vDirection * radius * sinf(lA)) * scale,
            (absoluteCapsule.b + vPerpendicular * radius * cosf(lB) - vDirection * radius * sinf(lB)) * scale,
            projectionView, thickness, debugColorWireframe);

        debug->renderLine(
            (absoluteCapsule.b + vSide * radius * cosf(lA) - vDirection * radius * sinf(lA)) * scale,
            (absoluteCapsule.b + vSide * radius * cosf(lB) - vDirection * radius * sinf(lB)) * scale,
            projectionView, thickness, debugColorWireframe);
    }
}