// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shapeGeometry.h"
#include "physics/physicsWorld.h"

ShapeGeometry::ShapeGeometry(Vector3 center, Geometry *geometry, PhysicsWorld *world) : Shape(center)
{
    this->geometry = geometry;
    vertexAmount = geometry->getVertexAmount();
    vertexes = new Vector3[vertexAmount];
    simScale = world->getSimScale();

    Matrix4 m;
    provideTransformation(&m);
}

Geometry *ShapeGeometry::getGeometry()
{
    return geometry;
}

ShapeCollisionType ShapeGeometry::getType(){
    return ShapeCollisionType::Polygon;
}

void ShapeGeometry::provideTransformation(Matrix4 *transformation)
{
    aabb = AABB(Vector3(FLT_MAX), Vector3(-FLT_MAX));

    auto data = geometry->getData();
    for (int i = 0; i < vertexAmount; i++)
    {
        int r = i * 3;
        Vector3 origin = Vector3(data[r], data[r + 1], data[r + 2]) * simScale;
        vertexes[i] = Vector3(*transformation * Vector4(origin, 1.0f));

        aabb.start.x = fminf(vertexes[i].x, aabb.start.x);
        aabb.start.y = fminf(vertexes[i].y, aabb.start.y);
        aabb.start.z = fminf(vertexes[i].z, aabb.start.z);
        aabb.end.x = fmaxf(vertexes[i].x, aabb.end.x);
        aabb.end.y = fmaxf(vertexes[i].y, aabb.end.y);
        aabb.end.z = fmaxf(vertexes[i].z, aabb.end.z);
    }
}

Vector3 ShapeGeometry::getClosestPoint(Vector3 point)
{
    int polyCount = vertexAmount / 3;
    float minDistance = MAXFLOAT;
    Vector3 out(0.0f);
    for (int i = 0; i < polyCount; i++)
    {
        int s = i * 3;
        Vector3 v1 = vertexes[s];
        Vector3 v2 = vertexes[s + 1];
        Vector3 v3 = vertexes[s + 2];

        Vector3 closest = getClosestPointOnTriangle(v1, v2, v3, point);
        float distance = glm::length(closest - point);
        if (distance < minDistance){
            minDistance = distance;
            out = closest;
        }
    }
    return out;
}

bool ShapeGeometry::testRay(Line line, std::vector<RayCollisionPoint> *points)
{
    int polyCount = vertexAmount / 3;
    float distance;
    Vector3 point;
    for (int i = 0; i < polyCount; i++)
    {
        int s = i * 3;
        if (testRayAgainstPolygon(vertexes[s], vertexes[s + 1], vertexes[s + 2], line, distance, point))
            points->push_back({point, distance});
    }
    return points->size() > 0;
}

AABB ShapeGeometry::getAABB()
{
    return aabb;
}

void ShapeGeometry::renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness)
{
    int polyCount = vertexAmount / 3;
    for (int i = 0; i < polyCount; i++)
    {
        int s = i * 3;
        debug->renderLine(vertexes[s] * scale, vertexes[s + 1] * scale, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));
        debug->renderLine(vertexes[s + 1] * scale, vertexes[s + 2] * scale, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));
        debug->renderLine(vertexes[s + 2] * scale, vertexes[s] * scale, projectionView, thickness, Vector3(0.9f, 0.9f, 0.9f));
    }
}