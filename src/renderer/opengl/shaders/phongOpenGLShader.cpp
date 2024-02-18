// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/shaders/phongOpenGLShader.h"
#include "renderer/opengl/textureOpenGL.h"
#include "renderer/opengl/shaderParameterOpenGL.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "renderer/opengl/glew.h"
#include "common/commonTextures.h"

extern const char *gShaderVertexCode;
extern const char *gShaderFragmentCode;

unsigned int PhongOpenGLShader::currentProgramm = 0;
unsigned int PhongOpenGLShader::tBlack = 0;
unsigned int PhongOpenGLShader::tGrey = 0;
unsigned int PhongOpenGLShader::tZeroNormal = 0;

PhongOpenGLShader::PhongOpenGLShader()
{
    setShaderCode(gShaderVertexCode, gShaderFragmentCode);
    build();
    bUseOwnShadowShader = true;
}

PhongOpenGLShader::PhongOpenGLShader(const std::string &vertexCode, const std::string &fragmentCode)
{
    setShaderCode(vertexCode, fragmentCode);
    build();
    bUseOwnShadowShader = true;
}

bool PhongOpenGLShader::build()
{
    unsigned int vertexShader = 0, fragmentShader = 0;
    if (!compile(GL_VERTEX_SHADER, vertexCode.c_str(), &vertexShader))
    {
        logger->logff("Unable to compile vertex shader:\n%s\n", vertexCode.c_str());
        return false;
    }

    if (!compile(GL_FRAGMENT_SHADER, fragCode.c_str(), &fragmentShader))
    {
        logger->logff("Unable to compile fragment shader:\n%s\n", fragCode.c_str());
        return false;
    }

    // Building usual pass programm
    programm = glCreateProgram();
    if (programm == -1)
        return false;

    glAttachShader(programm, fragmentShader);
    glAttachShader(programm, vertexShader);
    glLinkProgram(programm);
    glUseProgram(programm);

    locMModelViewProjection = glGetUniformLocation(programm, "mModelViewProjection");
    locMModel = glGetUniformLocation(programm, "mModel");
    locMNormal = glGetUniformLocation(programm, "mNormal");
    locTDefuse = glGetUniformLocation(programm, "TextureDefuse");
    locTNormal = glGetUniformLocation(programm, "TextureNormal");
    locTEmission = glGetUniformLocation(programm, "TextureEmission");
    locTRoughness = glGetUniformLocation(programm, "TextureRoughness");
    locUVControl = glGetUniformLocation(programm, "uvControl");
    locOpacity = glGetUniformLocation(programm, "fOpacity");

    tBlack = reinterpret_cast<TextureOpengGL *>(CommonTextures::getBlackTexture())->getGLTextureId();
    tGrey = reinterpret_cast<TextureOpengGL *>(CommonTextures::getGreyTexture())->getGLTextureId();
    tZeroNormal = reinterpret_cast<TextureOpengGL *>(CommonTextures::getZeroNormalTexture())->getGLTextureId();

    bIsReady = true;
    return true;
}

void PhongOpenGLShader::setTexture(TextureType type, Texture *texture)
{
    switch (type)
    {
    case TextureType::Albedo:
        tAlbedo = texture ? reinterpret_cast<TextureOpengGL *>(texture)->getGLTextureId() : 0;
        break;
    case TextureType::Normal:
        tNormal = texture ? reinterpret_cast<TextureOpengGL *>(texture)->getGLTextureId() : 0;
        break;
    case TextureType::Specular:
        tSpecular = texture ? reinterpret_cast<TextureOpengGL *>(texture)->getGLTextureId() : 0;
        break;
    case TextureType::Emission:
        tEmission = texture ? reinterpret_cast<TextureOpengGL *>(texture)->getGLTextureId() : 0;
        break;
    case TextureType::Roughness:
        tRoughness = texture ? reinterpret_cast<TextureOpengGL *>(texture)->getGLTextureId() : 0;
        break;
    case TextureType::Shadow:
        shadowTexture = texture;
        break;
    }
}

bool PhongOpenGLShader::use(Matrix4 &mModel, Matrix4 &mModelViewProjection)
{
    if (!bIsReady)
        build();
    if (!bIsReady)
        return false;

    if (currentShader != this || currentProgramm != programm)
    {
        currentShader = this;
        currentProgramm = programm;
        glUseProgram(programm);
    }

    auto mnMatrix = glm::transpose(glm::inverse(mModel));

    glUniformMatrix4fv(locMModelViewProjection, 1, GL_FALSE, value_ptr(mModelViewProjection));
    glUniformMatrix4fv(locMModel, 1, GL_FALSE, value_ptr(mModel));
    glUniformMatrix4fv(locMNormal, 1, GL_FALSE, value_ptr(mnMatrix));
    Vector4 defUV = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    glUniform4fv(locUVControl, 1, value_ptr(defUV));
    glUniform1f(locOpacity, opacity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tAlbedo ? tAlbedo : tGrey);
    glUniform1i(locTDefuse, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tEmission ? tEmission : tBlack);
    glUniform1i(locTEmission, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tNormal ? tNormal : tZeroNormal);
    glUniform1i(locTNormal, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tRoughness ? tRoughness : tGrey);
    glUniform1i(locTRoughness, 3);

    return true;
}

ShaderParameter *PhongOpenGLShader::createShaderParameter(const char *name, ShaderParameterType type)
{
    return new ShaderParameterOpenGL(this, name, type);
}

ShaderParameter *PhongOpenGLShader::createShaderUVParameter()
{
    return createShaderParameter("uvControl", ShaderParameterType::Float4);
}

void PhongOpenGLShader::destroyShaderParameter(ShaderParameter *parameter)
{
    delete parameter;
}

Texture *PhongOpenGLShader::getShadowTexture()
{
    return shadowTexture;
}

bool PhongOpenGLShader::compile(unsigned short type, const char *code, unsigned int *shader)
{
    int isCompiled;
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &code, NULL);
    glCompileShader(*shader);

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        showCompilationError(*shader);
        return false;
    }
    return true;
}

void PhongOpenGLShader::setShaderCode(const std::string &vertexCode, const std::string &fragmentCode)
{
    this->vertexCode = vertexCode;
    this->fragCode = fragmentCode;
}

// Straight go shader
const char *gShaderVertexCode =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "layout (location = 3) in vec3 aTangent;\n"
    "layout (location = 4) in vec3 aBitangent;\n"
    "uniform mat4 mModelViewProjection;\n"
    "uniform mat4 mModel;\n"
    "uniform mat4 mNormal;\n"
    "uniform vec4 uvControl;\n"
    "out vec2 TexCoords;\n"
    "out vec3 FragPos;\n"
    "out mat3 mTBN;\n"
    "void main() {\n"
    "   gl_Position = mModelViewProjection * vec4(aPos, 1.0);\n"
    "   FragPos = (mModel * vec4(aPos, 1.0)).xyz * 0.1;\n"
    "   TexCoords = vec2(uvControl.x, uvControl.y) + aTexCoord * vec2(uvControl.z, uvControl.w);\n"
    "   vec3 T = normalize((mNormal * vec4(aTangent,   0.0)).xyz);\n"
    "   vec3 B = normalize((mNormal * vec4(aBitangent, 0.0)).xyz);\n"
    "   vec3 N = normalize((mNormal * vec4(aNormal,    0.0)).xyz);\n"
    "   mTBN = mat3(T, B, N);\n"
    "}\n";

const char *gShaderFragmentCode =
    "#version 410 core\n"
    "layout (location = 0) out vec4 gAlbedo;\n"
    "layout (location = 1) out vec4 gNormal;\n"
    "layout (location = 2) out vec3 gPosition;\n"
    "layout (location = 3) out vec4 gEmission;\n"
    "uniform sampler2D TextureDefuse;\n"
    "uniform sampler2D TextureEmission;\n"
    "uniform sampler2D TextureNormal;\n"
    "uniform sampler2D TextureSpecular;\n"
    "uniform sampler2D TextureRoughness;\n"
    "uniform float fOpacity;\n"
    "in vec2 TexCoords;\n"
    "in vec3 FragPos;\n"
    "in mat3 mTBN;\n"
    "void main() {\n"
    "   gAlbedo = texture(TextureDefuse, TexCoords);\n"
    "   gAlbedo.a *= fOpacity;\n"
    "   gPosition = FragPos;\n"
    "   gNormal.rgb = normalize(mTBN * (texture(TextureNormal, TexCoords).rgb * 2.0 - 1.0));\n"
    "   gNormal.a = texture(TextureRoughness, TexCoords).r;\n"
    "   gEmission.rgba = texture(TextureEmission, TexCoords).rgba;\n"
    "   gEmission.a *= fOpacity;\n"
    "}\n";
