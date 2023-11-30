// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shaders/shader.h"
#include "shaders/rawShader.h"
#include "controller/resourceController.h"
#include "shaders/lightningShader.h"
#include "connector/withLogger.h"
#include "mesh/meshStatic.h"

class CommonShaders : public WithLogger
{
public:
    void build();

    EXPORT static Shader *getSpriteShader();
    EXPORT static Shader *getSpriteFrameShader();
    EXPORT static Shader *getScreenShader();
    EXPORT static RawShader *getEffectShader();
    EXPORT static RawShader *getInitialLightningShader();

    EXPORT static LightningShader *getSunShader();
    EXPORT static LightningShader *getSunWithShadowShader();
    EXPORT static LightningShader *getOmniShader();
    EXPORT static RawShader *getDebugCubeShader();

    inline static MeshStatic *getScreenMesh() { return screenMesh; }
    inline static MeshStatic *getSpriteMesh() { return spriteMesh; }
    inline static MeshStatic *getCubeMesh() { return cubeMesh; }

protected:
    static MeshStatic *spriteMesh;
    static MeshStatic *cubeMesh;
    static MeshStatic *screenMesh;

    static Shader *spriteShader;
    static Shader *spriteFrameShader;
    static Shader *screenShader;
    static RawShader *effectShader;
    static RawShader *initialLightningShader;
    static RawShader *debugCubeShader;

    static LightningShader *sunShader;
    static LightningShader *sunWithShadowShader;
    static LightningShader *omniShader;
};
