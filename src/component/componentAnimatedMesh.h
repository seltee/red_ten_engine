// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "mesh/mesh.h"
#include "mesh/meshCompound.h"
#include "mesh/meshStatic.h"
#include "common/utils.h"
#include "common/texture.h"
#include "component/component.h"
#include "shaders/shader.h"
#include "animation/animator.h"

class ComponentAnimatedMesh : public Component
{
public:
    EXPORT ComponentAnimatedMesh();

    EXPORT void process(float delta);

    EXPORT void onRender(Matrix4 &vpMatrix, Transformation *tf);
    EXPORT void onRenderShadow(Matrix4 &vpMatrix, Transformation *tf);

    EXPORT void setMesh(MeshCompound *mesh);
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix();

    EXPORT MeshStatic *getStaticMesh();

    EXPORT Animator *createAnimator(Animation *animation);

protected:
    MeshCompound *mesh = nullptr;
    Shader *shader = nullptr;
    std::vector<Animator *> animators;
};