// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "renderer/texture.h"
#include "component/component.h"
#include "renderer/shader.h"
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

    EXPORT void onRenderQueue(RenderQueue *renderQueue) override;

    EXPORT void setMesh(Mesh *mesh);
    EXPORT void setShader(Shader *shader);

    EXPORT Matrix4 getLocalspaceMatrix() override;

    EXPORT MeshStatic *getStaticMesh() override;

    EXPORT void addLod(Mesh *mesh, float distance);

    EXPORT inline void enableShadowCasting() { bCastShadows = true; };
    EXPORT inline void disableShadowCasting() { bCastShadows = false; };
    EXPORT inline bool isShadowCasting() { return bCastShadows; }

protected:
    Mesh *mesh = nullptr;
    Shader *shader = nullptr;

    std::vector<ComponentMeshLod> lods;
    bool bCastShadows = true;
};
