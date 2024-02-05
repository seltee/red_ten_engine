// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentAnimatedMesh.h"

ComponentAnimatedMesh::ComponentAnimatedMesh()
{
}

ComponentAnimatedMesh::~ComponentAnimatedMesh()
{
    deleteTransforms();
}

void ComponentAnimatedMesh::process(float delta)
{
    for (auto &animator : animators)
    {
        animator->process(delta);
    }
}

void ComponentAnimatedMesh::onRenderQueue(RenderQueue *renderQueue)
{
    if (mesh)
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix();
        mesh->prepareCache(&cache, animators, meshTransforms);
        mesh->queueAnimation(renderQueue, mModel, shader, &cache, bCastShadows);
    }
}

void ComponentAnimatedMesh::setMesh(MeshCompound *mesh)
{
    if (this->mesh != mesh)
    {
        deleteTransforms();

        this->mesh = mesh;
        if (owner)
            owner->childUpdated();
    }
}

void ComponentAnimatedMesh::setShader(Shader *shader)
{
    this->shader = shader;
    if (shader && !shader->isReady())
        shader->build();
}

Matrix4 ComponentAnimatedMesh::getLocalspaceMatrix()
{
    return *transform.getModelMatrix();
}

MeshStatic *ComponentAnimatedMesh::getStaticMesh()
{
    return mesh->getAsStatic();
}

Animator *ComponentAnimatedMesh::createAnimator(Animation *animation)
{
    auto animator = new Animator(animation);
    animators.push_back(animator);
    return animator;
}

Transformation *ComponentAnimatedMesh::getNodeTransformation(std::string name)
{
    if (!mesh)
        return nullptr;

    int amount = mesh->getMeshAmount();
    if (!meshTransforms && amount)
    {
        meshTransforms = new Transformation *[amount];
        for (int i = 0; i < amount; i++)
        {
            meshTransforms[i] = nullptr;
        }
        transformsAmount = amount;
    }

    if (meshTransforms && amount)
    {
        int index = mesh->getMeshIndex(name);
        if (index != -1)
        {
            if (!meshTransforms[index])
            {
                meshTransforms[index] = new Transformation();
            }
            return meshTransforms[index];
        }
    }
    return nullptr;
}

Matrix4 ComponentAnimatedMesh::getNodeMatrix(int index, bool includeParents, bool includeAnimation)
{
    if (!mesh)
        return Matrix4(1.0f);

    if (includeParents)
    {
        if (includeAnimation)
        {
            return *transform.getModelMatrix() * mesh->getAnimatedTransformationMatrixByIndex(index, &cache, animators, meshTransforms);
        }
        else
        {
            return *transform.getModelMatrix() * mesh->getTransformationMatrixWithParentingByIndex(index);
        }
    }
    else
    {
        return *transform.getModelMatrix() * mesh->getTransformationMatrixByIndex(index);
    }
}

int ComponentAnimatedMesh::getMeshNodeIndex(const std::string name)
{
    if (mesh)
    {
        return mesh->getMeshIndex(name);
    }
    return -1;
}

void ComponentAnimatedMesh::renderDebugVolume(Renderer *renderer, Matrix4 *mProjectionView, float thickness, Vector3 color)
{
    if (mesh && mesh->isRendarable())
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix();

        for (int i = 0; i < mesh->getMeshAmount(); i++)
        {
            auto nodeMesh = mesh->getMeshByIndex(i);
            Matrix4 mNode = mesh->getAnimatedTransformationMatrixByIndex(i, &cache, animators, meshTransforms);
            Matrix4 fullNodeTransform = mModel * mNode;

            auto volume = nodeMesh->getAsStatic()->getBoundVolumeSphere();
            float radius = volume->recalcRadius(&fullNodeTransform);
            Vector3 absoluteCenter = fullNodeTransform * Vector4(volume->center, 1.0f);

            for (int i = 0; i < 16; i++)
            {
                float lA = (float)i / 16.0f * CONST_PI * 2.0f;
                float lB = (float)(i + 1) / 16.0f * CONST_PI * 2.0f;

                renderer->renderDebugLine(
                    (absoluteCenter + Vector3(radius * sinf(lA), radius * cosf(lA), 0.0f)),
                    (absoluteCenter + Vector3(radius * sinf(lB), radius * cosf(lB), 0.0f)),
                    mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));

                renderer->renderDebugLine(
                    (absoluteCenter + Vector3(0.0f, radius * sinf(lA), radius * cosf(lA))),
                    (absoluteCenter + Vector3(0.0f, radius * sinf(lB), radius * cosf(lB))),
                    mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));

                renderer->renderDebugLine(
                    (absoluteCenter + Vector3(radius * cosf(lA), 0.0f, radius * sinf(lA))),
                    (absoluteCenter + Vector3(radius * cosf(lB), 0.0f, radius * sinf(lB))),
                    mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));
            }
        }
    }
}

void ComponentAnimatedMesh::deleteTransforms()
{
    if (meshTransforms)
    {
        for (int i = 0; i < transformsAmount; i++)
        {
            if (meshTransforms[i])
            {
                delete meshTransforms[i];
            }
        }
        delete[] meshTransforms;
        meshTransforms = nullptr;
    }
    transformsAmount = 0;
}