// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "mesh/mesh.h"
#include "mesh/meshStatic.h"
#include "renderer/texture.h"
#include "component/component.h"
#include "renderer/shader.h"
#include <string>

class ComponentMeshShell : public Component, WithRenderer
{
public:
    EXPORT ComponentMeshShell();
    EXPORT ~ComponentMeshShell();

    EXPORT void onRenderQueue(RenderQueue *renderQueue) override;
    EXPORT void process(float delta) override;

    EXPORT void setMesh(Mesh *mesh);
    EXPORT void setDensityTexture(Texture *texture);
    EXPORT void setDistorsionTexture(Texture *texture);
    EXPORT void setDistorsionUVShift(Vector2 shift);
    EXPORT void setRootTexture(Texture *texture);
    EXPORT void setTipTexture(Texture *texture);

    EXPORT void setParameters(int layersAmount, float layerHeight, float distorsionSpeed, float distorsionPower);

    EXPORT Matrix4 getLocalspaceMatrix() override;

    EXPORT MeshStatic *getStaticMesh() override;

    EXPORT void lookAt(Vector3 &point, bool bUseGlobalTranformation = true);

    EXPORT inline void enableShadowCasting() { bCastShadows = true; };
    EXPORT inline void disableShadowCasting() { bCastShadows = false; };
    EXPORT inline bool isShadowCasting() { return bCastShadows; }
    EXPORT inline void enableShapeCasting() { bRenderShape = true; };
    EXPORT inline void disableShapeCasting() { bRenderShape = false; };
    EXPORT inline bool isShapeCasting() { return bRenderShape; }

    EXPORT inline void setShadowShiftSize(Vector4 &uvShadowShiftSize) { this->uvShadowShiftSize = uvShadowShiftSize; }
    EXPORT inline Vector4 getShadowShiftSize() { return this->uvShadowShiftSize; }

protected:
    void clearGeometry();
    void recreate();

    Mesh *mesh = nullptr;
    Shader *shader = nullptr;

    bool bCastShadows = true;
    bool bRenderShape = true;

    Vector4 uvShadowShiftSize = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    int layersAmount = 0;
    float layerHeight = 0.0f;
    float distorsionSpeed = 0.01f;
    float distorsionPower = 0.01f;
    float distorsionUVShift[2] = {0.0f, 0.0f};

    ShaderParameter ***parametersList = nullptr;
    int parametersListAmount = 0;
    float *heights = nullptr;
    float *layerHeights = nullptr;
    float *alphaCheck = nullptr;

    Vector3 *color = nullptr;
    float time = 0.0f;

    TextureBinding *shellDataTextureBinding = nullptr;
    TextureBinding *shellDistorsionTextureBinding = nullptr;
    TextureBinding *shellRootTextureBinding = nullptr;
    TextureBinding *shellTipTextureBinding = nullptr;

    static Texture *defaultRootColor;
    static Texture *defaultTipColor;
};
