// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>

struct UV
{
    float x, y;
};

struct Vertex
{
    float p[3];
};

struct Normal
{
    float n[3];
};

struct Polygon
{
    Vertex p1;
    Vertex p2;
    Vertex p3;
    Normal n1;
    Normal n2;
    Normal n3;
    UV uv1;
    UV uv2;
    UV uv3;
};

class MeshDescriptor
{
public:
    MeshDescriptor();
    ~MeshDescriptor();

    void provideVertex(double *list, int countOfDoubles);
    void providePolygonIndexes(int *list, int countOfIndexes);
    void provideUVData(double *list, int countOfDoubles);
    void provideUVIndexes(int *list, int countOfIndexes);
    void provideNormals(double *list, int countOfDoubles);

    float *getAsFloatArray8f(int *amountOfFloats);

protected:
    Vertex *vertexes = nullptr;
    int vertexAmount = 0;

    UV *uvs = nullptr;
    int uvsAmount = 0;

    Normal *normals = nullptr;

    int *polygonIndexes = nullptr;
    int polygonIndexesAmount = 0;

    int *uvIndexes = nullptr;
    int uvIndexesAmount = 0;
};