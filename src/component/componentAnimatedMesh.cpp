// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "componentAnimatedMesh.h"

ComponentAnimatedMesh::ComponentAnimatedMesh()
{
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
        mesh->prepareCache(&cache, mModel, animators);
        mesh->queueAnimation(renderQueue, shader, &cache, bCastShadows);
    }
}

void ComponentAnimatedMesh::setMesh(MeshCompound *mesh)
{
    if (this->mesh != mesh)
    {
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