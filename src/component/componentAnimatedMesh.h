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
    EXPORT ~ComponentAnimatedMesh();

    EXPORT void process(float delta) override;

    EXPORT void onRenderQueue(RenderQueue *renderQueue) override;

    EXPORT void setMesh(MeshCompound *mesh);
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix() override;

    EXPORT MeshStatic *getStaticMesh() override;

    EXPORT Animator *createAnimator(Animation *animation);

    EXPORT Transformation *getNodeTransformation(std::string name);
    EXPORT Matrix4 getNodeMatrix(int index, bool includeParents, bool includeAnimation);
    EXPORT int getMeshNodeIndex(const std::string name);

    EXPORT inline void enableShadowCasting() { bCastShadows = true; };
    EXPORT inline void disableShadowCasting() { bCastShadows = false; };
    EXPORT inline bool isShadowCasting() { return bCastShadows; }

    EXPORT void renderDebugVolume(Renderer *renderer, Matrix4 *mProjectionView, float thickness, Vector3 color) override;
protected:
    void deleteTransforms();

    MeshCompound *mesh = nullptr;
    Shader *shader = nullptr;
    std::vector<Animator *> animators;
    MeshCompoundCache cache;
    bool bCastShadows = true;

    Transformation **meshTransforms = nullptr;
    int transformsAmount = 0;
};