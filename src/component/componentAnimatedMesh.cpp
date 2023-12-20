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
        mesh->prepareCache(&cache, mModel, animators, meshTransforms);
        mesh->queueAnimation(renderQueue, shader, &cache, bCastShadows);
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