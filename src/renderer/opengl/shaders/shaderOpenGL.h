// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/shader.h"

class ShaderOpenGL : public Shader
{
public:
    EXPORT ShaderOpenGL(const char *vertex, const char *fragment);

    EXPORT virtual bool build() override;
    EXPORT bool use(Matrix4 &mModel, Matrix4 &mModelViewProjection) override;
    EXPORT virtual bool compile(unsigned short type, const char *code, unsigned int *shader) override;

    EXPORT void setOpacity(float value) override;

    EXPORT int getUniformLocation(const char *name);
    EXPORT void provideFloatValue(int uniform, int amount, float *value);
    EXPORT void provideFloat2Value(int uniform, int amount, float *value);
    EXPORT void provideFloat3Value(int uniform, int amount, float *value);
    EXPORT void provideFloat4Value(int uniform, int amount, float *value);
    EXPORT void provideIntValue(int uniform, int amount, int *value);
    EXPORT void provideInt2Value(int uniform, int amount, int *value);
    EXPORT void provideInt3Value(int uniform, int amount, int *value);
    EXPORT void provideInt4Value(int uniform, int amount, int *value);

    EXPORT ShaderParameter *createShaderParameter(const char *name, ShaderParameterType type) override;
    EXPORT void destroyShaderParameter(ShaderParameter *parameter) override;

    int locMModelViewProjection;
    int locFOpacity;
    int locV3Normal;

protected:
    EXPORT void showCompilationError(unsigned int shader) override;

    const char *vertexCode = nullptr;
    const char *fragmentCode = nullptr;
};
