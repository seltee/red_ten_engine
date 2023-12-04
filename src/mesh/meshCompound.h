// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "mesh/meshCompoundCache.h"
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

    EXPORT void prepareCache(MeshCompoundCache *cache, Matrix4 &modelMatrix, std::vector<Animator *> animators);
    EXPORT void renderAnimation(Shader *shader, Matrix4 &vpMatrix, MeshCompoundCache *cache);

    EXPORT Mesh *createInstance();

    EXPORT MeshCompoundNode *addMesh(MeshStatic *mesh);
    EXPORT bool setParent(Mesh *child, Mesh *parent);

    EXPORT MeshStatic *getAsStatic();

protected:
    MeshCompoundNode *getNodeByMesh(Mesh *mesh);
    Matrix4 getTransformationMatrix(MeshCompoundNode *node);
    Matrix4 getAnimatedTransformationMatrix(MeshCompoundCache *cache, MeshCompoundNode *node, std::vector<Animator *> &animators);

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
