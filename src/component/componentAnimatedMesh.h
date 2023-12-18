// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "mesh/mesh.h"
#include "mesh/meshCompound.h"
#include "mesh/meshStatic.h"
#include "common/utils.h"
#include "renderer/texture.h"
#include "renderer/shader.h"
#include "component/component.h"
#include "animation/animator.h"
#include "mesh/meshCompoundCache.h"

class ComponentAnimatedMesh : public Component
{
public:
    EXPORT ComponentAnimatedMesh();

    EXPORT void process(float delta) override;

    EXPORT void onRenderQueue(RenderQueue *renderQueue) override;

    EXPORT void setMesh(MeshCompound *mesh);
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix() override;

    EXPORT MeshStatic *getStaticMesh() override;

    EXPORT Animator *createAnimator(Animation *animation);

    EXPORT inline void enableShadowCasting() { bCastShadows = true; };
    EXPORT inline void disableShadowCasting() { bCastShadows = false; };
    EXPORT inline bool isShadowCasting() { return bCastShadows; }

protected:
    MeshCompound *mesh = nullptr;
    Shader *shader = nullptr;
    std::vector<Animator *> animators;
    MeshCompoundCache cache;
    bool bCastShadows = true;
};