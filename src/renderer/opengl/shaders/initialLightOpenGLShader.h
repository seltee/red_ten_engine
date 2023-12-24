// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/texture.h"
#include <string>

class InitialLightOpenGLShader : public ShaderOpenGL
{
public:
    EXPORT InitialLightOpenGLShader();

    EXPORT void setTextureAlbedo(unsigned int albedoID);
    EXPORT void setTextureNormal(unsigned int normalID);
    EXPORT void setTexturePosition(unsigned int positionID);
    EXPORT void setTextureRadiance(unsigned int radianceID);
    EXPORT void setTextureEnvironment(unsigned int environmentID);
    EXPORT void setCameraPosition(Vector3 &position);
    EXPORT void setCameraDirection(Vector3 &direction);
    EXPORT void setAmbientColor(float *ambientColor);

protected:
    unsigned int locTDefuse;
    unsigned int locTNormal;
    unsigned int locTPosition;
    unsigned int locTRadiance;
    unsigned int locTEnvironment;
    unsigned int locCameraPosition;
    unsigned int locCameraDirection;
    unsigned int locAmbientColor;

    static const std::string internalVertexShader;
    static const std::string internalFragmentShader;
};