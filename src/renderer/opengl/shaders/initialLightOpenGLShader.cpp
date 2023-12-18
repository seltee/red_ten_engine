// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/shaders/initialLightOpenGLShader.h"
#include "renderer/opengl/glew.h"
#include "math/math.h"
#include "math/glm/gtc/type_ptr.hpp"

const char *InitialLightOpenGLShader::internalVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   texCoord = aTexCoord;\n"
    "}\n";

const char *InitialLightOpenGLShader::internalFragmentShader =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tAlbedo;\n"
    "uniform sampler2D tNormal;\n"
    "uniform sampler2D tPosition;\n"
    "uniform sampler2D tRadiance;\n"
    "uniform sampler2D tEnvironment;\n"
    "uniform vec3 vf3ambientColor;\n"
    "uniform vec3 cameraPos;\n"
    "vec2 texelSize = 1.0 / textureSize(tRadiance, 0);\n"
    ""
    "const vec2 invAtan = vec2(0.1591, 0.3183);\n"
    ""
    "vec2 SampleSphericalMap(vec3 v)\n"
    "{\n"
    "    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n"
    "    uv *= invAtan;\n"
    "    uv += 0.5;\n"
    "    return uv;\n"
    "}\n"
    ""
    "vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)\n"
    "{\n"
    "    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);\n"
    "}   \n"
    ""
    "vec3 RadianceCalculation(vec2 UV)\n"
    "{\n"
    "   vec3 radiance = vec3(0.0);\n"
    "   for(int x = -2; x <= 2; ++x){\n"
    "       for(int y = -2; y <= 2; ++y){\n"
    "           vec2 coords = UV + vec2(x, y) * texelSize;\n"
    "           radiance += texture(tRadiance, coords).rgb;\n"
    "       }\n"
    "   }\n"
    "   radiance /= 25.0;\n"
    "   return radiance;\n"
    "}\n"
    ""
    "void main() {\n"
    "   float AO = 1.0;\n"
    "   float Metallic = 0.0;\n"
    "   vec4 NormalData = texture(tNormal, texCoord);\n"
    "   vec3 Normal = NormalData.rgb;\n"
    "   if (length(Normal) == 0){\n"
    "       discard;\n"
    "   }\n"
    "   vec4 AlbedoData = texture(tAlbedo, texCoord);\n"
    "   vec3 Albedo = AlbedoData.rgb;\n"
    "   float AlbedoAlpha = AlbedoData.a;\n"
    "   float Roughness = NormalData.a;\n"
    "   vec3 FragPos = texture(tPosition, texCoord).rgb / 0.1;\n"
    "   const float HDRSwitch = 0.5;\n"
    "   vec3 F0 = vec3(0.04);\n"
    "   F0 = mix(F0, Albedo, Metallic);\n"
    "   vec3 V = normalize(cameraPos - FragPos);\n"
    "   vec3 kS = fresnelSchlickRoughness(max(dot(Normal, V), 0.0), F0, Roughness); \n"
    "   vec3 kD = 1.0 - kS;\n"
    "   vec2 uv = SampleSphericalMap(Normal);\n"
    "   vec2 uv2 = SampleSphericalMap(normalize(reflect(-V, Normal)));\n"
    "   vec3 IrradianceBlured = texture(tRadiance, vec2(0.0, 1.0) - uv).rgb;\n"
    "   vec3 IrradianceFull = texture(tEnvironment, vec2(0.0, 1.0) - uv2).rgb;\n"
    "   float fullRadianceFactor = max(pow(1.0 - Roughness, 2) - HDRSwitch, 0.0) * (1.0 / (1.0 - HDRSwitch));\n"
    "   float blurRadianceFactor = 1.0 - fullRadianceFactor;\n"
    "   vec3 Diffuse = (IrradianceBlured * blurRadianceFactor + IrradianceFull * fullRadianceFactor + vf3ambientColor) * Albedo.rgb;\n"
    "   vec3 Ambient = (kD * Diffuse) * AO;\n"
    "   FragColor = vec4(Ambient, AlbedoAlpha);\n"
    "}\n";

InitialLightOpenGLShader::InitialLightOpenGLShader() : ShaderOpenGL(internalVertexShader, internalFragmentShader)
{
    build();
    locTDefuse = getUniformLocation("tAlbedo");
    locTNormal = getUniformLocation("tNormal");
    locTPosition = getUniformLocation("tPosition");
    locTRadiance = getUniformLocation("tRadiance");
    locTEnvironment = getUniformLocation("tEnvironment");
    locCameraPosition = getUniformLocation("cameraPos");
    locAmbientColor = getUniformLocation("vf3ambientColor");
}

void InitialLightOpenGLShader::setTextureAlbedo(unsigned int albedoID)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoID);
    glUniform1i(locTDefuse, 0);
}

void InitialLightOpenGLShader::setTextureNormal(unsigned int normalID)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalID);
    glUniform1i(locTNormal, 1);
}

void InitialLightOpenGLShader::setTexturePosition(unsigned int positionID)
{
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, positionID);
    glUniform1i(locTPosition, 2);
}

void InitialLightOpenGLShader::setTextureRadiance(unsigned int radianceID)
{
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, radianceID);
    glUniform1i(locTRadiance, 3);
}

void InitialLightOpenGLShader::setTextureEnvironment(unsigned int environmentID)
{
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, environmentID);
    glUniform1i(locTEnvironment, 4);
}

void InitialLightOpenGLShader::setCameraPosition(Vector3 &position)
{
    glUniform3fv(locCameraPosition, 1, value_ptr(position));
}

void InitialLightOpenGLShader::setAmbientColor(float *ambientColor)
{
     glUniform3fv(locAmbientColor, 1, ambientColor);
}