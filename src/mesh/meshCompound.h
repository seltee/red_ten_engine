// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "math/transformation.h"
#include "animation/animator.h"
#include <vector>

class MeshCompoundNode;

class MeshCompound : public Mesh
{
public:
    EXPORT MeshCompound();
    EXPORT ~MeshCompound();

    EXPORT void render(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix);
    EXPORT void renderAnimation(Shader *shader, Matrix4 &vpMatrix, Matrix4 &modelMatrix, std::vector<Animator*> animators);
    EXPORT Mesh *createInstance();

    EXPORT MeshCompoundNode *addMesh(MeshStatic *mesh);
    EXPORT bool setParent(Mesh *child, Mesh *parent);

    EXPORT MeshStatic *getAsStatic();

protected:
    MeshCompoundNode *getNodeByMesh(Mesh *mesh);
    Matrix4 getTransformationMatrix(MeshCompoundNode *node);

    std::vector<MeshCompoundNode *> nodes;
    MeshStatic *meshStatic = nullptr;
};

class MeshCompoundNode
{
public:
    MeshCompoundNode(MeshStatic *mesh) { this->mesh = mesh; }
    MeshStatic *mesh;
    Transformation transform;
    MeshCompoundNode *parent = nullptr;
};
