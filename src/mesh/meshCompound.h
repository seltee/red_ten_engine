// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "mesh/meshCompoundCache.h"
#include "math/transformation.h"
#include "animation/animator.h"
#include "connector/withRenderer.h"
#include "renderer/renderQueue.h"
#include <vector>

class MeshCompoundNode;

class MeshCompound : public Mesh, WithRenderer
{
public:
    EXPORT MeshCompound();
    EXPORT ~MeshCompound();

    EXPORT void render();
    EXPORT void prepareCache(MeshCompoundCache *cache, Matrix4 &modelMatrix, std::vector<Animator *> animators, Transformation **indexTransformations);
    EXPORT void queueAnimation(RenderQueue *renderQueue, Shader *shader, MeshCompoundCache *cache, bool bCastShadows);

    EXPORT Mesh *createInstance();

    EXPORT MeshCompoundNode *addMesh(MeshStatic *mesh);
    EXPORT bool setParent(Mesh *child, Mesh *parent);
    EXPORT bool setParent(std::string &child, std::string &parent);

    EXPORT MeshStatic *getAsStatic();

    EXPORT inline int getMeshAmount() { return static_cast<int>(nodes.size()); }

    // -1 if not found
    EXPORT int getMeshIndex(std::string name);

protected:
    MeshCompoundNode *getNodeByMesh(Mesh *mesh);
    MeshCompoundNode *getNodeByName(std::string &name);
    Matrix4 getTransformationMatrix(MeshCompoundNode *node);
    Matrix4 getAnimatedTransformationMatrix(MeshCompoundCache *cache, MeshCompoundNode *node, std::vector<Animator *> &animators, Transformation **indexTransformations);

    std::vector<MeshCompoundNode *> nodes;
    MeshStatic *meshStatic = nullptr;
};

class MeshCompoundNode
{
public:
    MeshCompoundNode(MeshStatic *mesh, int index) { this->mesh = mesh; this->index = index; }
    Transformation transform;
    MeshStatic *mesh = nullptr;
    MeshCompoundNode *parent = nullptr;
    int index = -1;
};
