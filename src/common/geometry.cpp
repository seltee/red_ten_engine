// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "geometry.h"
#include <string>

Geometry::Geometry(const float *data, int vertexAmount, int floatsPerVertex, int inVertexShiftToPosition)
{
    this->vertexAmount = vertexAmount;
    geometry = (float *)malloc(vertexAmount * 3 * sizeof(float));
    for (int i = 0; i < vertexAmount; i++)
    {
        memcpy(&geometry[i * 3], &data[i * floatsPerVertex + inVertexShiftToPosition], 3 * sizeof(float));
        // TODO: Temporary FBX fix
        geometry[i * 3 + 0] *= 0.01f;
        geometry[i * 3 + 1] *= 0.01f;
        geometry[i * 3 + 2] *= 0.01f;
    }
}

Geometry::~Geometry()
{
    free(geometry);
}

const float *Geometry::getData()
{
    return geometry;
}

int Geometry::getVertexAmount()
{
    return vertexAmount;
}