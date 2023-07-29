// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"

class Hull
{
public:
    Hull(Vector3 *verticies, int amount);

    HullPolygon addPolygon(int *points, int numOfVerticies);
    void updateAbsoluteForm(Matrix4 *transformation, AABB *aabb);
    void rebuildEdges();

    void rebuildNormals();

    Vector3 *verticies = nullptr;
    Vector3 *absoluteVerticies = nullptr;
    int amountOfVertices = 0;

    std::vector<HullEdge> edges;
    std::vector<HullPolygon> polies;

    Vector3 hullCenter = Vector3(0.0f);
};