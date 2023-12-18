// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/shaderParameter.h"

class ShaderOpenGL;

class ShaderParameterOpenGL : public ShaderParameter
{
public:
    EXPORT ShaderParameterOpenGL(Shader *shader, const char *name, ShaderParameterType type);
    EXPORT ~ShaderParameterOpenGL();

    EXPORT void apply() override;

    EXPORT inline int getUniformLoc() { return uniformLoc; }

protected:
    int uniformLoc = -1;
    ShaderOpenGL *shader = nullptr;
};