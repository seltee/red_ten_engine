// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "common/utils.h"

struct HullPolygonSimple
{
    int *points;
    int numbOfVerticies;
};

class Hull
{
public:
    EXPORT Hull(Vector3 *verticies, int amount, float simScale = 1.0f);

    EXPORT HullPolygon addPolygon(int *points, int numOfVerticies);
    EXPORT void addPolygons(std::vector<HullPolygonSimple> *polygons);

    EXPORT void updateAbsoluteForm(Matrix4 *transformation, AABB *aabb);
    EXPORT void rebuildEdges();

    EXPORT void rebuildNormals();

    EXPORT bool checkConvexity();

    Vector3 *verticies = nullptr;
    Vector3 *absoluteVerticies = nullptr;
    int amountOfVertices = 0;

    std::vector<HullEdge> edges;
    std::vector<HullPolygon> polies;

    Vector3 hullCenter = Vector3(0.0f);

protected:
    bool inline isVertexInPolygon(int v, HullPolygon *p)
    {
        for (int i = 0; i < p->pointsAmount; i++)
        {
            if (v == p->points[i])
                return true;
        }
        return false;
    }
};