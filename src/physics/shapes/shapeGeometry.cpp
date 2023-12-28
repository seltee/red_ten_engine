// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeGeometry.h"
#include "physics/physicsWorld.h"

ShapeGeometry::ShapeGeometry(Vector3 center, Geometry *geometry, PhysicsWorld *world) : Shape(center)
{
    this->geometry = geometry;
    verticiesAmount = geometry->getVertexAmount();
    verticies = new Vector3[verticiesAmount];
    simScale = world->getSimScale();

    Matrix4 m;
    provideTransformation(&m);
}

Geometry *ShapeGeometry::getGeometry()
{
    return geometry;
}

ShapeCollisionType ShapeGeometry::getType()
{
    return ShapeCollisionType::Geometry;
}

void ShapeGeometry::provideTransformation(Matrix4 *transformation)
{
    aabb = AABB(Vector3(FLT_MAX), Vector3(-FLT_MAX));

    auto data = geometry->getData();
    for (int i = 0; i < verticiesAmount; i++)
    {
        int r = i * 3;
        Vector3 origin = Vector3(data[r], data[r + 1], data[r + 2]) * simScale;
        verticies[i] = Vector3(*transformation * Vector4(origin, 1.0f));

        aabb.start.x = fminf(verticies[i].x, aabb.start.x);
        aabb.start.y = fminf(verticies[i].y, aabb.start.y);
        aabb.start.z = fminf(verticies[i].z, aabb.start.z);
        aabb.end.x = fmaxf(verticies[i].x, aabb.end.x);
        aabb.end.y = fmaxf(verticies[i].y, aabb.end.y);
        aabb.end.z = fmaxf(verticies[i].z, aabb.end.z);
    }
}

Vector3 ShapeGeometry::getClosestPoint(const Vector3 &point)
{
    int triCount = verticiesAmount / 3;
    float minDistance = FLT_MAX;
    Vector3 out(0.0f);
    for (int i = 0; i < triCount; i++)
    {
        int s = i * 3;
        Vector3 closest = getClosestPointOnTriangle(&verticies[s], point);
        float distance = glm::length(closest - point);
        if (distance < minDistance)
        {
            minDistance = distance;
            out = closest;
        }
    }
    return out;
}

float ShapeGeometry::getClosestPoint(const Segment &segment, Vector3 &onSegment, Vector3 &onGeometry)
{
    int triCount = verticiesAmount / 3;
    float minDistance = FLT_MAX;
    for (int i = 0; i < triCount; i++)
    {
        int s = i * 3;
        Vector3 newOnSegment, newOnGeometry;
        float distance = segment.getClosestPointToTriangle(&verticies[s], newOnGeometry, newOnSegment);
        if (distance < minDistance)
        {
            minDistance = distance;
            onSegment = newOnSegment;
            onGeometry = newOnGeometry;
        }
    }
    return minDistance;
}

bool ShapeGeometry::testRay(const Segment &line, std::vector<RayCollisionPoint> *points)
{
    int polyCount = verticiesAmount / 3;
    float distance;
    Vector3 point;
    for (int i = 0; i < polyCount; i++)
    {
        int s = i * 3;
        if (testRayAgainstTriangle(&verticies[s], line, distance, point))
        {
            Vector3 normal = getPolygonNormal(verticies[s], verticies[s + 1], verticies[s + 2]);
            points->push_back({point, normal, distance});
        }
    }
    return !points->empty();
}

AABB ShapeGeometry::getAABB()
{
    return aabb;
}

void ShapeGeometry::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness)
{
    int polyCount = verticiesAmount / 3;
    for (int i = 0; i < polyCount; i++)
    {
        int s = i * 3;
        debug->renderLine(verticies[s] * scale, verticies[s + 1] * scale, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));
        debug->renderLine(verticies[s + 1] * scale, verticies[s + 2] * scale, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));
        debug->renderLine(verticies[s + 2] * scale, verticies[s] * scale, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));
    }
}