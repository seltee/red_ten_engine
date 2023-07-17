// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shaders/shader.h"
#include "shaders/rawShader.h"
#include "controller/resourceController.h"
#include "shaders/lightningShader.h"
#include "connector/withLogger.h"
#include "common/mesh.h"

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

    EXPORT static Mesh *getScreenMesh();
    EXPORT static Mesh *getSpriteMesh();
    EXPORT static Mesh *getCubeMesh();

protected:
    static Mesh *spriteMesh;
    static Mesh *cubeMesh;
    static Mesh *screenMesh;

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
