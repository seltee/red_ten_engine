// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shader.h"

Shader *Shader::currentShader = nullptr;

bool Shader::build()
{
    return false;
}

bool Shader::use(Matrix4 &mModel, Matrix4 &mModelViewProjection)
{
    return false;
}

bool Shader::useShadow(Matrix4 &mModel, Matrix4 &mModelViewProjection)
{
    return false;
}

bool Shader::compile(unsigned short type, const char *code, unsigned int *shader)
{
    return false;
}

void Shader::showCompilationError(unsigned int shader)
{
}

ShaderParameter *Shader::createShaderParameter(const char *name, ShaderParameterType type)
{
    return nullptr;
}

void Shader::destroyShaderParameter(ShaderParameter *parameter)
{
}
