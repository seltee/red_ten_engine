// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/phongShader.h"
#include "renderer/texture.h"

class PhongOpenGLShader : public PhongShader
{
public:
    EXPORT PhongOpenGLShader();
    EXPORT PhongOpenGLShader(const char *vertexCode, const char *fragCode, const char *shadowVertexCode, const char *shadowFragCode);
    EXPORT bool build() override;
    EXPORT void setTexture(TextureType type, Texture *texture) override;
    EXPORT bool use(Matrix4 &mModel, Matrix4 &mModelViewProjection) override;
    EXPORT bool useShadow(Matrix4 &mModel, Matrix4 &mModelViewProjection) override;

protected:
    EXPORT bool compile(unsigned short type, const char *code, unsigned int *shader) override;
    EXPORT void setShaderCode(const char *vertexCode, const char *fragCode, const char *shadowVertexCode, const char *shadowFragCode);

    const char *vertexCode = nullptr;
    const char *fragCode = nullptr;
    const char *shadowVertexCode = nullptr;
    const char *shadowFragCode = nullptr;

    int locMModelViewProjection = 0;
    int locMModel = 0;
    int locMNormal = 0;
    int locTDefuse = 0;
    int locTNormal = 0;
    int locTEmission = 0;
    int locTRoughness = 0;

    unsigned int locMShadowModelViewProjection = 0;
    unsigned int tAlbedo = 0;
    unsigned int tNormal = 0;
    unsigned int tSpecular = 0;
    unsigned int tEmission = 0;
    unsigned int tRoughness = 0;

    static unsigned int currentProgramm;
    static unsigned int tBlack;
    static unsigned int tGrey;
    static unsigned int tZeroNormal;
};