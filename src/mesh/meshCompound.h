// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "math/transformation.h"
#include <vector>

class MeshCompoundNode;

class MeshCompound : public Mesh
{
public:
    EXPORT MeshCompound();
    EXPORT ~MeshCompound();

    EXPORT void render(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix);
    EXPORT Mesh *createInstance();

    EXPORT MeshCompoundNode *addMesh(MeshStatic *mesh);

    EXPORT MeshStatic *getAsStatic();

protected:
    std::vector<MeshCompoundNode *> nodes;
    MeshStatic *meshStatic = nullptr;
};

class MeshCompoundNode
{
public:
    MeshCompoundNode(MeshStatic *mesh) { this->mesh = mesh; }
    MeshStatic *mesh;
    Transformation transform;
};
