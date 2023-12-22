// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/shader.h"
#include "renderer/texture.h"

enum class TextureType
{
    Albedo = 0,
    Normal = 1,
    Specular = 2,
    Emission = 3,
    Roughness = 4,
    Shadow = 5,
};

class PhongShader : public Shader
{
protected:
    EXPORT PhongShader();

public:
    EXPORT virtual void setTexture(TextureType type, Texture *texture);
    EXPORT virtual bool use(Matrix4 &mModel, Matrix4 &mModelViewProjection) override;

    // Note - should be destroyed with destroyShaderParameter when no longer in use or component is deleted
    EXPORT virtual ShaderParameter *createShaderUVParameter();
};