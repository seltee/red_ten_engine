// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shaders/shader.h"
#include "shaders/rawShader.h"
#include "controller/resourceController.h"
#include "shaders/lightningShader.h"
#include "common/withLogger.h"
#include "common/mesh.h"

class CommonShaders : public WithLogger
{
public:
    static void build();

    EXPORT static Shader *getSpriteShader();
    EXPORT static Shader *getSpriteFrameShader();
    EXPORT static Shader *getScreenShader();
    EXPORT static RawShader *getEffectShader();
    EXPORT static RawShader *getInitialLightningShader();

    EXPORT static LightningShader *getSunShader();
    EXPORT static LightningShader *getSunWithShadowShader();
    EXPORT static LightningShader *getOmniShader();

    EXPORT static Mesh *getScreenMesh();
    EXPORT static Mesh *getSpriteMesh();
protected:
    static Mesh *spriteMesh;
    static Mesh *screenMesh;

    static Shader *spriteShader;
    static Shader *spriteFrameShader;
    static Shader *screenShader;
    static RawShader *effectShader;
    static RawShader *initialLightningShader;

    static LightningShader *sunShader;
    static LightningShader *sunWithShadowShader;
    static LightningShader *omniShader;
};
