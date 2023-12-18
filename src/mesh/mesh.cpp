// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "mesh/mesh.h"
#include "mesh/meshStatic.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::render()
{
}

Mesh *Mesh::createInstance()
{
    return nullptr;
}

Geometry *Mesh::getGeometry()
{
    return nullptr;
}

MeshStatic *Mesh::getAsStatic()
{
    return nullptr;
}