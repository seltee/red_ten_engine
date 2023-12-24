// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shader.h"

Shader *Shader::currentShader = nullptr;

Shader::~Shader()
{
}

bool Shader::build()
{
    return false;
}

bool Shader::use(Matrix4 &mModel, Matrix4 &mModelViewProjection)
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

Texture *Shader::getShadowTexture()
{
    return nullptr;
}

void Shader::setUVShiftSize(Vector4 &v)
{
}

TextureBinding *Shader::addTextureBinding(const std::string parameterName)
{
    return nullptr;
}