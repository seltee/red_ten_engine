// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/shaderParameter.h"
#include "mesh/meshStatic.h"
#include "physics/physicsBody.h"
#include <vector>

class Actor;

#define MAX_RENDER_ELEMENTS 128000
#define MAX_LIGHTS 16000
#define MAX_DEBUG_ELEMENTS 1000

struct RenderElement
{
    Matrix4 mModelViewProjection;
    Matrix4 mModel;
    ColorMode colorMode;
    Shader *shader;
    Texture *texture;
    MeshStatic *mesh;
    float opacity;
    ShaderParameter **parameters;
    int parametersAmount;
    Vector4 uvShiftSize;
};

struct RenderElementLight
{
    LightType type;
    Vector3 position;
    Vector3 color;
    float affectDistance;
    bool bCastShadows;
};

struct RenderElementDebug
{
    PhysicsBody *body;
    Actor *actor;
    float symScale;
    float lineThickness;
};

class RenderQueue
{
public:
    EXPORT RenderQueue();

    EXPORT void reset();
    EXPORT inline void setViewProjectionMatrix(Matrix4 &mViewProjection) { this->mViewProjection = mViewProjection; }
    EXPORT inline Matrix4 *getViewProjectionMatrix() { return &mViewProjection; }
    EXPORT inline void setViewMatrix(Matrix4 &mView) { this->mView = mView; }
    EXPORT inline Matrix4 *getViewMatrix() { return &mView; }

    EXPORT void addMainPhase(Matrix4 &mModel, Shader *shader, Texture *texture, MeshStatic *mesh, ShaderParameter **parameters, int parametersAmount);
    EXPORT void addBlendingPhase(Matrix4 &mModel, ColorMode colorMode, Shader *shader, Texture *texture, MeshStatic *mesh, float opacity, ShaderParameter **parameters, int parametersAmount);
    EXPORT void addShadowCaster(Matrix4 &mModel, MeshStatic *mesh, Texture *texture, Vector4 &uvShiftSize);
    EXPORT void addLight(LightType type, Vector3 position, Vector3 color, float affectDistance, bool bCastShadows);

    EXPORT void addDebugBody(PhysicsBody *body, float symScale, float lineThickness);
    EXPORT void addDebugActor(Actor *actor, float lineThickness);

    inline void enableSorting() { bUseSort = true; };
    inline void disableSorting() { bUseSort = false; }
    inline bool isUsingSorting() { return bUseSort; };

    inline RenderElement **getMainPhaseElements() { return mainPhaseElements; }
    inline int getMainPhaseElementsAmount() { return lastElementMainPhase; }

    inline RenderElement **getBlendingPhaseElements() { return blendPhaseElements; }
    inline int getBlendingPhaseElementsAmount() { return lastElementBlendPhase; }

    inline RenderElement **getShadowCasterElements() { return shadowCasterElements; }
    inline int getShadowCasterElementsAmount() { return lastShadowCasterElement; }

    inline RenderElementLight *getLightElements() { return lightElements; }
    inline int getLightElementsAmount() { return lastElementLight; }

    inline RenderElementDebug *getDebugElements() { return debugElements; }
    inline int getDebugElementsAmount() { return lastDebugElement; }

    inline void setAmbientLight(Vector3 &ambientLight) { this->ambientLight = ambientLight; }
    inline Vector3 getAmbientLight() { return this->ambientLight; }

    inline void setCameraPosition(Vector3 &cameraPosition) { this->cameraPosition = cameraPosition; }
    inline Vector3 getCameraPosition() { return this->cameraPosition; }

    inline void setCameraDirection(Vector3 &cameraDirection) { this->cameraDirection = cameraDirection; }
    inline Vector3 getCameraDirection() { return this->cameraDirection; }

    inline void setHDRRadianceTexture(Texture *texture) { this->HDRRadianceTexture = texture; }
    inline Texture *getHDRRadianceTexture() { return this->HDRRadianceTexture; }

    inline void setHDRTexture(Texture *texture) { this->HDRTexture = texture; }
    inline Texture *getHDRTexture() { return this->HDRTexture; }

    inline void setGamma(float gamma) { this->gamma = gamma; }
    inline float getGamma() { return this->gamma; }

    inline void setShowEnvHDR(bool bValue) { this->bShowEnvHDR = bValue; }
    inline bool isShowingEnvHDR() { return this->bShowEnvHDR; }

    inline void setEnvHDRRotation(float rotation) { this->envHDRRotation = rotation; }
    inline float getEnvHDRRotation() { return this->envHDRRotation; }

    inline void setCullingPlanes(Vector4 *cullingPlanes) { this->cullingPlanes = cullingPlanes; }
    inline Vector4 *getCullingPlanes() { return this->cullingPlanes; }

    inline void setUseCameraDirectionForLights(bool state) { this->bUseCameraDirectionForLights = state; }
    inline bool isUsingCameraDirectionForLights() { return this->bUseCameraDirectionForLights; }

    std::atomic<bool> bDone = false;

protected:
    Matrix4 mViewProjection;
    Matrix4 mView;

    RenderElement renderElements[MAX_RENDER_ELEMENTS];
    std::atomic<int> lastElement = 0;

    RenderElement *mainPhaseElements[MAX_RENDER_ELEMENTS];
    std::atomic<int> lastElementMainPhase = 0;

    RenderElement *blendPhaseElements[MAX_RENDER_ELEMENTS];
    int lastElementBlendPhase = 0;

    RenderElement *shadowCasterElements[MAX_RENDER_ELEMENTS];
    int lastShadowCasterElement = 0;

    RenderElementLight lightElements[MAX_LIGHTS];
    int lastElementLight = 0;

    RenderElementDebug debugElements[MAX_DEBUG_ELEMENTS];
    int lastDebugElement = 0;

    bool bUseSort = false;
    bool bUseCameraDirectionForLights = false;
    Vector3 ambientLight;
    Vector3 cameraPosition;
    Vector3 cameraDirection;
    Texture *HDRRadianceTexture = nullptr;
    Texture *HDRTexture = nullptr;
    float gamma = 1.0f;

    bool bShowEnvHDR = false;
    float envHDRRotation = 0.0f;

    Vector4 *cullingPlanes = nullptr;
};