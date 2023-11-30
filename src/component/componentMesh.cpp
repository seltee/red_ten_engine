// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentMesh.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "common/commonShaders.h"
#include "opengl/glew.h"
#include <math.h>

ComponentMesh::ComponentMesh() : Component()
{
    shader = nullptr;
}

void ComponentMesh::onRender(Matrix4 &vpMatrix, Transformation *tf)
{
    if (mesh)
    {
        Matrix4 mModelTransform = *tf->getModelMatrix() * *transform.getModelMatrix();
        prepareColorMode();
        mesh->render(shader, vpMatrix, mModelTransform);
    }
}

void ComponentMesh::onRenderShadow(Matrix4 &vpMatrix, Transformation *tf)
{
    if (mesh)
    {
        Matrix4 mModelTransform = *tf->getModelMatrix() * *transform.getModelMatrix();
        mesh->render(shader, vpMatrix, mModelTransform);
    }
}

void ComponentMesh::setMesh(Mesh *mesh)
{
    if (this->mesh != mesh)
    {
        this->mesh = mesh;
        if (owner)
            owner->childUpdated();
    }
}

void ComponentMesh::setShader(Shader *shader)
{
    this->shader = shader;
    if (shader && !shader->isReady())
        shader->build();
}

Matrix4 ComponentMesh::getLocalspaceMatrix()
{
    return *transform.getModelMatrix();
}

MeshStatic *ComponentMesh::getStaticMesh()
{
    return mesh->getAsStatic();
}