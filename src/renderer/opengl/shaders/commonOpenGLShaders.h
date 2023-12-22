// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/opengl/shaders/initialLightOpenGLShader.h"
#include "renderer/opengl/shaders/lightningOpenGLShader.h"
#include "renderer/opengl/shaders/cubeMapOpenGLShader.h"
#include "controller/resourceController.h"
#include "connector/withLogger.h"
#include "mesh/meshStatic.h"

class Renderer;

class CommonOpenGLShaders : public WithLogger
{
public:
    void build(Renderer *renderer);

    EXPORT static ShaderOpenGL *getSpriteShader();
    EXPORT static ShaderOpenGL *getSpriteFrameShader();
    EXPORT static ShaderOpenGL *getScreenShader();
    EXPORT static ShaderOpenGL *getEffectShader();
    EXPORT static ShaderOpenGL *getGammaShader();
    EXPORT static ShaderOpenGL *getGammaFXAAShader();
    EXPORT static ShaderOpenGL *getSimpleShadowShader();
    EXPORT static ShaderOpenGL *getTexturedShadowShader();

    EXPORT static InitialLightOpenGLShader *getInitialLightShader();
    EXPORT static LightningOpenGLShader *getSunShader();
    EXPORT static LightningOpenGLShader *getSunWithShadowShader();
    EXPORT static LightningOpenGLShader *getOmniShader();

    EXPORT static ShaderOpenGL *getDebugCubeShader();

    EXPORT static CubeMapOpenGLShader *getCubeMapShader();

    inline static MeshStatic *getScreenMesh() { return screenMesh; }
    inline static MeshStatic *getSpriteMesh() { return spriteMesh; }
    inline static MeshStatic *getCubeMesh() { return cubeMesh; }

protected:
    static MeshStatic *spriteMesh;
    static MeshStatic *cubeMesh;
    static MeshStatic *screenMesh;

    static ShaderOpenGL *spriteShader;
    static ShaderOpenGL *spriteFrameShader;
    static ShaderOpenGL *screenShader;
    static ShaderOpenGL *gammaShader;
    static ShaderOpenGL *gammaFXAAShader;
    static ShaderOpenGL *effectShader;
    static ShaderOpenGL *simpleShadowShader;
    static ShaderOpenGL *texturedShadowShader;


    static ShaderOpenGL *debugCubeShader;
    static InitialLightOpenGLShader *initialLightShader;
    static CubeMapOpenGLShader *cubeMapShader;

    static LightningOpenGLShader *sunShader;
    static LightningOpenGLShader *sunWithShadowShader;
    static LightningOpenGLShader *omniShader;
};
