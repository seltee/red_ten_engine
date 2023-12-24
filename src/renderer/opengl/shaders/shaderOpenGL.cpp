// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/shaders/shaderOpenGL.h"
#include "renderer/opengl/glew.h"
#include "renderer/opengl/shaderParameterOpenGL.h"
#include "renderer/opengl/textureBindingOpenGL.h"
#include "math/glm/gtc/type_ptr.hpp"
#include <string.h>

extern const int slotLinking[];

ShaderOpenGL::ShaderOpenGL(const std::string &vertex, const std::string &fragment)
{
    this->vertexCode = vertex;
    this->fragmentCode = fragment;
}

ShaderOpenGL::~ShaderOpenGL()
{
}

bool ShaderOpenGL::build()
{
    if (!vertexCode.empty() && !fragmentCode.empty())
    {
        unsigned int vertexShader = 0, fragmentShader = 0;
        if (!compile(GL_VERTEX_SHADER, vertexCode.c_str(), &vertexShader))
        {
            logger->logff("Unable to compile vertex shader:\n%s\n", vertexCode.c_str());
            return false;
        }

        if (!compile(GL_FRAGMENT_SHADER, fragmentCode.c_str(), &fragmentShader))
        {
            logger->logff("Unable to compile fragment shader:\n%s\n", fragmentCode.c_str());
            return false;
        }

        programm = glCreateProgram();
        if (programm != -1)
        {
            glAttachShader(programm, fragmentShader);
            glAttachShader(programm, vertexShader);
            glLinkProgram(programm);

            glValidateProgram(programm);
            int params;
            glGetProgramiv(programm, GL_VALIDATE_STATUS, &params);
            if (params != GL_TRUE)
            {
                logger->logff("Shader validation failed:\nVertex shader:\n%s\nFragment shader:\n%s\n", vertexCode.c_str(), fragmentCode.c_str());
                return false;
            }

            locMModelViewProjection = glGetUniformLocation(programm, "mModelViewProjection");
            locMModel = glGetUniformLocation(programm, "mModel");
            locMNormal = glGetUniformLocation(programm, "mNormal");
            locFOpacity = glGetUniformLocation(programm, "fOpacity");
            locUVShiftSize = glGetUniformLocation(programm, "v4uvShiftSize");

            bIsReady = true;
            return true;
        }
        logger->logff("Unable to create program:\nVertex shader:\n%s\nFragment shader:\n%s\n", vertexCode.c_str(), fragmentCode.c_str());
        return false;
    }
    return false;
}

bool ShaderOpenGL::use(Matrix4 &mModel, Matrix4 &mModelViewProjection)
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

    if (locMModelViewProjection != -1)
    {
        glUniformMatrix4fv(locMModelViewProjection, 1, GL_FALSE, value_ptr(mModelViewProjection));
    }
    if (locMModel != -1)
    {
        glUniformMatrix4fv(locMModel, 1, GL_FALSE, value_ptr(mModel));
    }
    if (locMNormal != -1)
    {
        auto mnMatrix = glm::transpose(glm::inverse(mModel));
        glUniformMatrix4fv(locMNormal, 1, GL_FALSE, value_ptr(mnMatrix));
    }

    int slot = 0;
    for (auto &binding : bindings)
    {
        glActiveTexture(slotLinking[slot]);
        glBindTexture(GL_TEXTURE_2D, binding->textureId);
        glUniform1i(binding->locTexture, slot);

        slot++;
    }

    return true;
}

bool ShaderOpenGL::compile(unsigned short type, const char *code, unsigned int *shader)
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

TextureBinding *ShaderOpenGL::addTextureBinding(std::string parameterName)
{
    TextureBindingOpenGL *newBinding = new TextureBindingOpenGL();
    newBinding->locTexture = getUniformLocation(parameterName.c_str());
    bindings.push_back(newBinding);
    return newBinding;
}

void ShaderOpenGL::setOpacity(float value)
{
    this->opacity = value;
    glUniform1f(locFOpacity, value);
}

int ShaderOpenGL::getUniformLocation(const char *name)
{
    if (programm != -1)
        return glGetUniformLocation(programm, name);
    return -1;
}

void ShaderOpenGL::provideFloatValue(int uniform, int amount, float *value)
{
    if (programm != -1)
        glUniform1fv(uniform, amount, value);
}

void ShaderOpenGL::provideFloat2Value(int uniform, int amount, float *value)
{
    if (programm != -1)
        glUniform2fv(uniform, amount, value);
}

void ShaderOpenGL::provideFloat3Value(int uniform, int amount, float *value)
{
    if (programm != -1)
        glUniform3fv(uniform, amount, value);
}

void ShaderOpenGL::provideFloat4Value(int uniform, int amount, float *value)
{
    if (programm != -1)
        glUniform4fv(uniform, amount, value);
}

void ShaderOpenGL::provideIntValue(int uniform, int amount, int *value)
{
    if (programm != -1)
        glUniform1iv(uniform, amount, value);
}

void ShaderOpenGL::provideInt2Value(int uniform, int amount, int *value)
{
    if (programm != -1)
        glUniform2iv(uniform, amount, value);
}

void ShaderOpenGL::provideInt3Value(int uniform, int amount, int *value)
{
    if (programm != -1)
        glUniform3iv(uniform, amount, value);
}

void ShaderOpenGL::provideInt4Value(int uniform, int amount, int *value)
{
    if (programm != -1)
        glUniform4iv(uniform, amount, value);
}

ShaderParameter *ShaderOpenGL::createShaderParameter(const char *name, ShaderParameterType type)
{
    return new ShaderParameterOpenGL(this, name, type);
}

void ShaderOpenGL::showCompilationError(unsigned int shader)
{
    char log[4096];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    logger->logff("Shader Compilation Error:\n%s\n", log);
}

void ShaderOpenGL::destroyShaderParameter(ShaderParameter *parameter)
{
    if (parameter)
    {
        delete parameter;
    }
}

void ShaderOpenGL::setUVShiftSize(Vector4 &v)
{
    if (programm != -1)
        glUniform4fv(locUVShiftSize, 1, value_ptr(v));
}

const int slotLinking[] = {
    GL_TEXTURE0,
    GL_TEXTURE1,
    GL_TEXTURE2,
    GL_TEXTURE3,
    GL_TEXTURE4,
    GL_TEXTURE5,
    GL_TEXTURE6,
    GL_TEXTURE7,
    GL_TEXTURE8,
    GL_TEXTURE9,
    GL_TEXTURE10,
    GL_TEXTURE11,
    GL_TEXTURE12,
    GL_TEXTURE13,
    GL_TEXTURE14,
    GL_TEXTURE15,
    GL_TEXTURE16,
    GL_TEXTURE17,
    GL_TEXTURE18,
    GL_TEXTURE19,
    GL_TEXTURE20,
    GL_TEXTURE21,
    GL_TEXTURE22,
    GL_TEXTURE23,
    GL_TEXTURE24,
    GL_TEXTURE25,
    GL_TEXTURE26,
    GL_TEXTURE27,
    GL_TEXTURE28,
    GL_TEXTURE29,
    GL_TEXTURE30,
    GL_TEXTURE31};