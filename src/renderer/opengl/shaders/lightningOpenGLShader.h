// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/opengl/shaders/shaderOpenGL.h"

class LightningOpenGLShader : public ShaderOpenGL
{
public:
    EXPORT LightningOpenGLShader(const std::string &vertex, const std::string &fragment);

    EXPORT bool use(Matrix4 &mModel, Matrix4 &mModelViewProjection) override;
    EXPORT void setLightColor(Vector3 &v);
    EXPORT void setLightDirection(Vector3 &v);
    EXPORT void setAffectDistance(float value);
    EXPORT void setLightSpaceMatrix(Matrix4 &mLightSpace);
    EXPORT void setCameraPosition(Vector3 &v);
    EXPORT void setCameraDirection(Vector3 &v);
    EXPORT void setLightPosition(Vector3 &v);

    int locV3Position;
    int locMLightSpace;

    int locV3LightColor;
    int locV3LightDirection;
    int locFAffectDistance;

    int locV3CameraPosition;
    int locV3CameraDirection;

    int locTGAlbedoSpec;
    int locTGNormal;
    int locTGPosition;
    int locTShadowMap;
    int locTEnvMap;
};
