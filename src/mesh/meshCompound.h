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

    EXPORT void queueAnimation(RenderQueue *renderQueue, Matrix4 &modelMatrix, Shader *shader, MeshCompoundCache *cache, bool bCastShadows);

    EXPORT void prepareCache(MeshCompoundCache *cache, std::vector<Animator *> animators, Transformation **indexTransformations);
    EXPORT void processNodeEntry(MeshCompoundCache *cache, MeshCompoundCacheEntry *nodeEntry, std::vector<Animator *> &animators, Transformation **indexTransformations);

    EXPORT Mesh *createInstance() override;

    EXPORT MeshCompoundNode *addMesh(MeshStatic *mesh);
    EXPORT bool setParent(Mesh *child, Mesh *parent);
    EXPORT bool setParent(std::string &child, std::string &parent);

    EXPORT MeshStatic *getAsStatic() override;
    EXPORT bool isRendarable() override;

    EXPORT inline int getMeshAmount() { return static_cast<int>(nodes.size()); }

    // -1 if not found
    EXPORT int getMeshIndex(std::string name);
    EXPORT MeshStatic *getMeshByIndex(int index);

    Matrix4 getTransformationMatrixByIndex(int index);
    Matrix4 getTransformationMatrixWithParentingByIndex(int index);
    Matrix4 getAnimatedTransformationMatrixByIndex(int index, MeshCompoundCache *cache, std::vector<Animator *> &animators, Transformation **indexTransformations);

protected:
    MeshCompoundNode *getNodeByMesh(Mesh *mesh);
    MeshCompoundNode *getNodeByName(std::string &name);
    Matrix4 getTransformationMatrix(MeshCompoundNode *node);
    // with parenting
    Matrix4 getAnimatedTransformationMatrix(MeshCompoundCache *cache, MeshCompoundNode *node, std::vector<Animator *> &animators, Transformation **indexTransformations);
    // without parenting
    Matrix4 getLocalTransformationMatrix(MeshCompoundNode *node, std::vector<Animator *> &animators, Transformation **indexTransformations);

    std::vector<MeshCompoundNode *> nodes;
    MeshStatic *meshStatic = nullptr;

    Vector4 uvShiftSize = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
};

class MeshCompoundNode
{
public:
    MeshCompoundNode(MeshStatic *mesh, int index)
    {
        this->mesh = mesh;
        this->index = index;
    }
    Transformation transform;
    MeshStatic *mesh = nullptr;
    MeshCompoundNode *parent = nullptr;
    int index = -1;
};
