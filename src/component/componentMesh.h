// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "component/component.h"
#include "resource/texture.h"
#include "shaders/shader.h"
#include "resource/mesh.h"

class ComponentMesh : public Component
{
public:
    EXPORT ComponentMesh();

    EXPORT bool onRenderPrepare(Matrix4 &vpMatrix, Transformation *tf, bool isShadowStage);
    EXPORT int getVertexAmount();

    EXPORT void setMesh(Mesh *mesh);
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix();

protected:
    Mesh *mesh = nullptr;
    Shader *shader = nullptr;
};
