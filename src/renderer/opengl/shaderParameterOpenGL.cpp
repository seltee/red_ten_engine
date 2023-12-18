// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/shaderParameterOpenGL.h"
#include "renderer/opengl/shaders/shaderOpenGL.h"

ShaderParameterOpenGL::ShaderParameterOpenGL(Shader *shader, const char *name, ShaderParameterType type) : ShaderParameter(type)
{
    this->shader = reinterpret_cast<ShaderOpenGL *>(shader);

    uniformLoc = this->shader->getUniformLocation(name);
}

ShaderParameterOpenGL::~ShaderParameterOpenGL()
{
}

void ShaderParameterOpenGL::apply()
{
    if (uniformLoc != -1 && shader && data && amount > 0)
    {
        switch (type)
        {
        case ShaderParameterType::Float:
            shader->provideFloatValue(uniformLoc, amount, reinterpret_cast<float *>(data));
            break;
        case ShaderParameterType::Float2:
            shader->provideFloat2Value(uniformLoc, amount, reinterpret_cast<float *>(data));
            break;
        case ShaderParameterType::Float3:
            shader->provideFloat3Value(uniformLoc, amount, reinterpret_cast<float *>(data));
            break;
        case ShaderParameterType::Float4:
            shader->provideFloat4Value(uniformLoc, amount, reinterpret_cast<float *>(data));
            break;
        case ShaderParameterType::Int:
            shader->provideIntValue(uniformLoc, amount, reinterpret_cast<int *>(data));
            break;
        case ShaderParameterType::Int2:
            shader->provideInt2Value(uniformLoc, amount, reinterpret_cast<int *>(data));
            break;
        case ShaderParameterType::Int3:
            shader->provideInt3Value(uniformLoc, amount, reinterpret_cast<int *>(data));
            break;
        case ShaderParameterType::Int4:
            shader->provideInt4Value(uniformLoc, amount, reinterpret_cast<int *>(data));
            break;
        default:
            break;
        }
    }
}