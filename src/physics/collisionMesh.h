#pragma once
#include "math/math.h"
#include <vector>

struct PolygonIndexs
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
};

class CollisionMesh
{
public:
    CollisionMesh();

    void setPoints(Vector3 *points, int amount);
    void setIndexes(PolygonIndexs *indexes, int amount);

    void update(Matrix4 *m);
    std::vector<Triangle> *getPolygons();

    bool testCollision(CollisionMesh *foreignMesh);

protected:
    Vector4 *points = nullptr;
    int pointsAmount = 0;
    PolygonIndexs *indexes = nullptr;
    int indexesAmount = 0;

    std::vector<Triangle> polygons;
};
