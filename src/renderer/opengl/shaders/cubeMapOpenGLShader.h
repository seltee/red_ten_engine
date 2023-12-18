// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/texture.h"

class CubeMapOpenGLShader : public ShaderOpenGL
{
public:
    EXPORT CubeMapOpenGLShader();
    EXPORT void setCubeMap(Texture *texture);

protected:
    unsigned int locTCubeMap = 0;

    static const char *internalVertexShader;
    static const char *internalFragmentShader;
};