// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "mesh/meshStatic.h"
#include "math/math.h"
#include <string.h>

MeshStatic::MeshStatic()
{
}

MeshStatic::~MeshStatic()
{
}

Geometry *MeshStatic::getGeometry()
{
    return nullptr;
}

float *MeshStatic::getVertexData()
{
    return vertexData;
}

int MeshStatic::getVertexAmount()
{
    return vertexAmount;
}

int MeshStatic::getFloatsPerVertex()
{
    return floatsPerVertex;
}

int MeshStatic::getAttributesAmount()
{
    return attributesAmount;
}

void MeshStatic::setupFloatsArray(const float *data, int vertexAmount, int attributesAmount, int *attributeSize, bool buildTangents)
{
}

EXPORT void MeshStatic::setOtherMeshAsInstanceOfThis(MeshStatic *mesh)
{
}

void MeshStatic::clear()
{
}

void MeshStatic::useVertexArray()
{
}

MeshStatic *MeshStatic::getAsStatic()
{
    return this;
}