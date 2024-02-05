// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "mesh/meshCompound.h"
#include "renderer/renderer.h"
#include <algorithm>

MeshCompound::MeshCompound()
{
}

MeshCompound::~MeshCompound()
{
}

void MeshCompound::queueAnimation(RenderQueue *renderQueue, Matrix4 &modelMatrix, Shader *shader, MeshCompoundCache *cache, bool bCastShadows)
{
    for (auto &entry : cache->entriesToRender)
    {
        MeshStatic *mesh = entry.node->mesh->getAsStatic();
        Matrix4 mNodeModel = modelMatrix * entry.model;

        renderQueue->addMainPhase(mNodeModel, shader, nullptr, mesh, nullptr, 0);
        if (bCastShadows)
            renderQueue->addShadowCaster(mNodeModel, mesh, nullptr, uvShiftSize);
    }
}

void MeshCompound::prepareCache(MeshCompoundCache *cache, std::vector<Animator *> animators, Transformation **indexTransformations)
{
    cache->makeDirty();
    for (auto node : nodes)
    {
        if (node->mesh->isRendarable())
        {
            auto nodeEntry = cache->getEntryByNode(node);
            if (!nodeEntry)
            {
                nodeEntry = cache->addEntry(node);
            }

            if (nodeEntry->isDirty)
                processNodeEntry(cache, nodeEntry, animators, indexTransformations);
        }
    }
}

void MeshCompound::processNodeEntry(MeshCompoundCache *cache, MeshCompoundCacheEntry *nodeEntry, std::vector<Animator *> &animators, Transformation **indexTransformations)
{
    nodeEntry->model = getLocalTransformationMatrix(nodeEntry->node, animators, indexTransformations);

    if (nodeEntry->node->parent)
    {
        auto parentEntry = cache->getEntryByNode(nodeEntry->node->parent);
        if (!parentEntry)
            parentEntry = cache->addEntry(nodeEntry->node->parent);

        if (parentEntry->isDirty)
            processNodeEntry(cache, parentEntry, animators, indexTransformations);

        nodeEntry->model = parentEntry->model * nodeEntry->model;
    }

    nodeEntry->isDirty = false;
}

Mesh *MeshCompound::createInstance()
{
    MeshCompound *newMesh = new MeshCompound;
    for (auto &it : nodes)
    {
        newMesh->addMesh(it->mesh);
    }
    return newMesh;
}

MeshCompoundNode *MeshCompound::addMesh(MeshStatic *mesh)
{
    auto newNode = new MeshCompoundNode(mesh, static_cast<int>(nodes.size()));
    nodes.push_back(newNode);
    return newNode;
}

bool MeshCompound::setParent(Mesh *child, Mesh *parent)
{
    MeshCompoundNode *childNode = getNodeByMesh(child);
    MeshCompoundNode *parentNode = getNodeByMesh(parent);

    if (childNode && parentNode)
    {
        childNode->parent = parentNode;
        return true;
    }
    return false;
}

bool MeshCompound::setParent(std::string &child, std::string &parent)
{
    MeshCompoundNode *childNode = getNodeByName(child);
    MeshCompoundNode *parentNode = getNodeByName(parent);

    if (childNode && parentNode)
    {
        childNode->parent = parentNode;
        return true;
    }
    return false;
}

MeshStatic *MeshCompound::getAsStatic()
{
    if (meshStatic)
        return meshStatic;

    if (nodes.size() == 0)
        return nullptr;

    meshStatic = getRenderer()->createStaticMesh();

    // Collect amount of verticies
    int amountOfVerticies = 0;
    for (auto &node : nodes)
    {
        amountOfVerticies += node->mesh->getVertexAmount();
    }

    // vertex - 3, normals - 3, UVs - 2
    const int floatPerVertex = 8;
    float *data = new float[floatPerVertex * amountOfVerticies];
    memset(data, 0, floatPerVertex * amountOfVerticies * sizeof(float));

    // copying data
    int dataShift = 0;
    for (auto &node : nodes)
    {
        int meshFloatsPerVertex = node->mesh->getFloatsPerVertex();
        int meshVertexAmount = node->mesh->getVertexAmount();
        float *meshVertexData = node->mesh->getVertexData();

        // Ignore old tangents cause they will be outdated with apply of transformation
        auto modelMatrix = getTransformationMatrix(node);
        auto rq = node->transform.getRotation();

        for (int i = 0; i < meshVertexAmount; i++)
        {
            int fromShift = i * meshFloatsPerVertex;
            int toShift = i * floatPerVertex + dataShift;

            Vector4 vp = Vector4(meshVertexData[fromShift], meshVertexData[fromShift + 1], meshVertexData[fromShift + 2], 1.0f);
            Vector3 vn = Vector3(meshVertexData[fromShift + 3], meshVertexData[fromShift + 4], meshVertexData[fromShift + 5]);

            Vector4 mvp = modelMatrix * vp;
            Vector3 mvr = glm::normalize(glm::rotate(rq, vn));

            data[toShift] = mvp.x;
            data[toShift + 1] = mvp.y;
            data[toShift + 2] = mvp.z;
            data[toShift + 3] = mvr.x;
            data[toShift + 4] = mvr.y;
            data[toShift + 5] = mvr.z;
            data[toShift + 6] = meshVertexData[fromShift + 6];
            data[toShift + 7] = meshVertexData[fromShift + 7];
        }

        dataShift += meshVertexAmount * floatPerVertex;
    }

    int attributeSizes[3] = {3, 3, 2};
    meshStatic->setupFloatsArray(data, amountOfVerticies, 3, attributeSizes, true);

    delete[] data;
    return meshStatic;
}

bool MeshCompound::isRendarable()
{
    return nodes.size() > 0;
}

int MeshCompound::getMeshIndex(std::string name)
{
    int i = 0;
    for (auto &it : nodes)
    {
        if (it->mesh->getName() == name)
            return i;
        i++;
    }
    return -1;
}

MeshStatic *MeshCompound::getMeshByIndex(int index)
{
    if (index >= 0 && index < nodes.size())
    {
        MeshCompoundNode *node = nodes.at(index);
        if (node)
            return node->mesh;
    }
    return nullptr;
}

Matrix4 MeshCompound::getTransformationMatrixByIndex(int index)
{
    if (index >= 0 && index < nodes.size())
    {
        MeshCompoundNode *node = nodes.at(index);
        return *node->transform.getModelMatrix();
    }
    return Matrix4(1.0f);
}

Matrix4 MeshCompound::getTransformationMatrixWithParentingByIndex(int index)
{
    if (index >= 0 && index < nodes.size())
    {
        MeshCompoundNode *node = nodes.at(index);
        return getTransformationMatrix(node);
    }
    return Matrix4(1.0f);
}

Matrix4 MeshCompound::getAnimatedTransformationMatrixByIndex(int index, MeshCompoundCache *cache, std::vector<Animator *> &animators, Transformation **indexTransformations)
{
    if (index >= 0 && index < nodes.size())
    {
        MeshCompoundNode *node = nodes.at(index);
        return getAnimatedTransformationMatrix(cache, node, animators, indexTransformations);
    }
    return Matrix4(1.0f);
}

MeshCompoundNode *MeshCompound::getNodeByMesh(Mesh *mesh)
{
    for (auto &it : nodes)
    {
        if (it->mesh == mesh)
        {
            return it;
        }
    }
    return nullptr;
}

MeshCompoundNode *MeshCompound::getNodeByName(std::string &name)
{
    for (auto &it : nodes)
    {
        if (it->mesh->getName() == name)
        {
            return it;
        }
    }
    return nullptr;
}

Matrix4 MeshCompound::getTransformationMatrix(MeshCompoundNode *node)
{
    if (!node->parent)
    {
        return *node->transform.getModelMatrix();
    }
    else
    {
        Matrix4 out(1.0f);
        std::vector<Matrix4 *> list;
        MeshCompoundNode *current = node;
        while (current)
        {
            list.push_back(current->transform.getModelMatrix());
            current = current->parent;
        }

        for (auto it = list.rbegin(); it != list.rend(); ++it)
        {
            out = out * **it;
        }

        return out;
    }
}

Matrix4 MeshCompound::getAnimatedTransformationMatrix(MeshCompoundCache *cache, MeshCompoundNode *node, std::vector<Animator *> &animators, Transformation **indexTransformations)
{
    AnimationTarget *target = nullptr;
    Animator *animator = nullptr;
    for (auto anim : animators)
    {
        auto animation = anim->getAnimation();
        if (animation)
        {
            target = animation->getTargetByName(node->mesh->getName());
            if (target)
            {
                animator = anim;
                break;
            }
        }
    }

    if (!target || !animator)
    {
        if (indexTransformations)
        {
            Matrix4 local;
            if (node->parent)
                local = getAnimatedTransformationMatrix(cache, node->parent, animators, indexTransformations) * *node->transform.getModelMatrix();
            else
                local = getTransformationMatrix(node);
            if (indexTransformations[node->index])
            {
                local *= *indexTransformations[node->index]->getModelMatrix();
            }
            return local;
        }
        else
        {
            if (node->parent)
                return getAnimatedTransformationMatrix(cache, node->parent, animators, indexTransformations) * *node->transform.getModelMatrix();
            else
                return getTransformationMatrix(node);
        }
    }

    Transformation transformation;
    target->getTransformByTime(animator->time, &transformation);
    if (indexTransformations)
    {
        Matrix4 local;
        if (!node->parent)
        {
            local = *transformation.getModelMatrix();
        }
        else
        {
            Matrix4 parentTransformation = getAnimatedTransformationMatrix(cache, node->parent, animators, indexTransformations);
            local = parentTransformation * *transformation.getModelMatrix();
        }
        if (indexTransformations[node->index])
        {
            local *= *indexTransformations[node->index]->getModelMatrix();
        }
        return local;
    }
    else
    {
        if (!node->parent)
        {
            return *transformation.getModelMatrix();
        }
        else
        {
            Matrix4 parentTransformation = getAnimatedTransformationMatrix(cache, node->parent, animators, indexTransformations);
            return parentTransformation * *transformation.getModelMatrix();
        }
    }
}

Matrix4 MeshCompound::getLocalTransformationMatrix(MeshCompoundNode *node, std::vector<Animator *> &animators, Transformation **indexTransformations)
{
    AnimationTarget *target = nullptr;
    Animator *animator = nullptr;
    for (auto anim : animators)
    {
        auto animation = anim->getAnimation();
        if (animation)
        {
            target = animation->getTargetByName(node->mesh->getName());
            if (target)
            {
                animator = anim;
                break;
            }
        }
    }

    if (!target || !animator)
    {
        if (indexTransformations)
        {
            Matrix4 local = *node->transform.getModelMatrix();
            if (indexTransformations[node->index])
                local *= *indexTransformations[node->index]->getModelMatrix();
            return local;
        }
        else
        {
            return *node->transform.getModelMatrix();
        }
    }

    Transformation transformation;
    target->getTransformByTime(animator->time, &transformation);
    if (indexTransformations)
    {
        Matrix4 local = *transformation.getModelMatrix();
        if (indexTransformations[node->index])
            local *= *indexTransformations[node->index]->getModelMatrix();
        return local;
    }
    else
    {
        return *transformation.getModelMatrix();
    }

    return Matrix4(1.0f);
}