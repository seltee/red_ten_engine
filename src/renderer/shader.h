// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "common/utils.h"
#include "connector/withLogger.h"
#include "renderer/shaderParameter.h"
#include "renderer/texture.h"

class Shader : public WithLogger
{
public:
    EXPORT virtual bool build();
    EXPORT virtual bool use(Matrix4 &mModel, Matrix4 &mModelViewProjection);

    EXPORT virtual void setOpacity(float value) { this->opacity = value; }
    EXPORT inline float getOpacity() { return opacity; }

    EXPORT virtual bool compile(unsigned short type, const char *code, unsigned int *shader);
    EXPORT bool isReady() { return bIsReady; }

    // Note - should be destroyed with destroyShaderParameter when no longer in use or component is deleted
    EXPORT virtual ShaderParameter *createShaderParameter(const char *name, ShaderParameterType type);
    EXPORT virtual void destroyShaderParameter(ShaderParameter *parameter);

    EXPORT virtual Texture *getShadowTexture();

    EXPORT virtual void setUVShiftSize(Vector4 &v);

protected:
    EXPORT virtual void showCompilationError(unsigned int shader);

    static Shader *currentShader;

    bool bIsReady = false;
    bool bUseOwnShadowShader = false;
    unsigned int programm = -1;
    unsigned int shadowProgramm = -1;
    float opacity = 1.0f;
};
