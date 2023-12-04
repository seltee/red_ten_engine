// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "common/texture.h"
#include "component/component.h"
#include "shaders/shader.h"
#include <vector>

struct ComponentMeshLod
{
    Mesh *mesh;
    float distance;
};

class ComponentMesh : public Component
{
public:
    EXPORT ComponentMesh();

    EXPORT void onRender(Matrix4 &vpMatrix, Transformation *tf);
    EXPORT void onRenderShadow(Matrix4 &vpMatrix, Transformation *tf);

    EXPORT void setMesh(Mesh *mesh);
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix();

    EXPORT MeshStatic *getStaticMesh();

    EXPORT void addLod(Mesh *mesh, float distance);

protected:
    Mesh *mesh = nullptr;
    Shader *shader = nullptr;

    std::vector<ComponentMeshLod> lods;
};
