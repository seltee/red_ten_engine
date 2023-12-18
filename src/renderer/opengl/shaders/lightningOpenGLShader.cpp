// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/shaders/lightningOpenGLShader.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "renderer/opengl/glew.h"
#include <string.h>

LightningOpenGLShader::LightningOpenGLShader(const char *vertex, const char *fragment) : ShaderOpenGL(vertex, fragment)
{
    build();
    locV3Position = glGetUniformLocation(programm, "v3Position");
    locMLightSpace = glGetUniformLocation(programm, "mlightSpace");

    locV3LightColor = glGetUniformLocation(programm, "lightColor");
    locV3LightDirection = glGetUniformLocation(programm, "lightDir");
    locFAffectDistance = glGetUniformLocation(programm, "affectDistance");
    locV3CameraPos = glGetUniformLocation(programm, "cameraPos");

    locTGAlbedoSpec = glGetUniformLocation(programm, "tAlbedoSpec");
    locTGNormal = glGetUniformLocation(programm, "tNormal");
    locTGPosition = glGetUniformLocation(programm, "tPosition");
    locTShadowMap = glGetUniformLocation(programm, "tShadowMap");
    locTEnvMap = glGetUniformLocation(programm, "tEnvironment");
}

bool LightningOpenGLShader::use(Matrix4 &mModel, Matrix4 &mModelViewProjection)
{
    if (!bIsReady)
        build();
    if (!bIsReady)
        return false;

    if (currentShader != this)
    {
        currentShader = this;
        glUseProgram(programm);
    }

    if (locTGAlbedoSpec != -1)
        glUniform1i(locTGAlbedoSpec, 0);
    if (locTGNormal != -1)
        glUniform1i(locTGNormal, 1);
    if (locTGPosition != -1)
        glUniform1i(locTGPosition, 2);
    if (locTShadowMap != -1)
        glUniform1i(locTShadowMap, 3);
    if (locTEnvMap != -1)
        glUniform1i(locTEnvMap, 4);

    if (locMModelViewProjection != -1)
        glUniformMatrix4fv(locMModelViewProjection, 1, GL_FALSE, value_ptr(mModelViewProjection));

    return true;
}

void LightningOpenGLShader::setLightColor(Vector3 &v)
{
    if (locV3LightColor != -1)
        glUniform3fv(locV3LightColor, 1, value_ptr(v));
}

void LightningOpenGLShader::setLightDirection(Vector3 &v)
{
    if (locV3LightDirection != -1)
        glUniform3fv(locV3LightDirection, 1, value_ptr(v));
}

void LightningOpenGLShader::setAffectDistance(float value)
{
    if (locFAffectDistance != -1)
        glUniform1f(locFAffectDistance, value);
}
void LightningOpenGLShader::setLightSpaceMatrix(Matrix4 &mLightSpace)
{
    if (locMLightSpace != -1)
        glUniformMatrix4fv(locMLightSpace, 1, GL_FALSE, value_ptr(mLightSpace));
}

void LightningOpenGLShader::setCameraPosition(Vector3 &v)
{
    if (locV3CameraPos != -1)
        glUniform3fv(locV3CameraPos, 1, value_ptr(v));
}

void LightningOpenGLShader::setLightPosition(Vector3 &v)
{
    if (locV3Position != -1)
        glUniform3fv(locV3Position, 1, value_ptr(v));
}