#pragma once
#include "common/utils.h"
#include "math/math.h"
#include "shape.h"
#include "physics/hull.h"
#include "connector/withDebug.h"
#include <string>

struct FaceQuery
{
    float separation;
    HullPolygon *polygon;
    Vector3 axis;
};

struct EdgeQuery
{
    float separation;
    HullEdge *edgeA;
    HullEdge *edgeB;
    Vector3 axis;
};

class ShapeConvex : public Shape,
                    public WithDebug
{
public:
    ShapeConvex(Vector3 center) : Shape(center) {}
    EXPORT virtual Hull *getHull() { return hull; }

    EXPORT Hull *setNewHull(Vector3 *verticies, int amount);

    EXPORT void provideTransformation(Matrix4 *transformation);
    EXPORT void updateTransformation();

    EXPORT virtual ShapeCollisionType getType();

    EXPORT bool testRay(Line line, std::vector<RayCollisionPoint> *points);

    EXPORT AABB getAABB();

    EXPORT FaceQuery queryFaceDirection(ShapeConvex *foreignShape);
    EXPORT EdgeQuery queryEdgeDirection(ShapeConvex *foreignShape);

    EXPORT Vector3 findFurthestPoint(Vector3 inDirection);
    EXPORT Vector3 getClosestPointToHull(Vector3 point);

    EXPORT void renderDebug(Matrix4 *projectionView, Matrix4 *model, float scale, float thickness);
    EXPORT inline Vector3 getHullCenter() { return getHull()->hullCenter; }

protected:
    Hull *hull = nullptr;
    Matrix4 transformation;
    bool isDirty = true;
    AABB aabb;
};
