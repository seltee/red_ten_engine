// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/phongShader.h"
#include "renderer/opengl/textureOpenGL.h"
#include "renderer/opengl/glew.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "common/commonTextures.h"

PhongShader::PhongShader()
{
}

void PhongShader::setTexture(TextureType type, Texture *texture)
{
}

bool PhongShader::use(Matrix4 &mModel, Matrix4 &mModelViewProjection)
{
    return false;
}

ShaderParameter *PhongShader::createShaderUVParameter()
{
    return nullptr;
}