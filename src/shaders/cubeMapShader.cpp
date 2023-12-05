// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "cubeMapShader.h"
#include "opengl/glew.h"

const char *CubeMapShader::internalVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 mTransform;\n"
    "uniform mat4 mViewProjection;\n"
    "out vec3 localPos;\n"
    "void main() {\n"
    "   gl_Position = mViewProjection * mTransform * vec4(aPos, 1.0);\n"
    "   localPos = aPos\n;"
    "}\n";

const char *CubeMapShader::internalFragmentShader =
    "#version 410 core\n"
    "layout (location = 0) out vec4 gAlbedo;"
    "layout (location = 1) out vec3 gNormal;"
    "layout (location = 2) out vec3 gPosition;"
    "layout (location = 3) out vec3 gEmission;"
    "in vec3 localPos;\n"
    "uniform sampler2D tCubeMap;\n"
    "const vec2 invAtan = vec2(0.1591, 0.3183);\n"
    ""
    "vec2 SampleSphericalMap(vec3 v)\n"
    "{\n"
    "    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n"
    "    uv *= invAtan;\n"
    "    uv += 0.5;\n"
    " return uv;\n"
    "}\n"
    ""
    "void main() {\n"
    "   vec3 Normal = normalize(localPos);\n"
    "   vec2 UV = SampleSphericalMap(Normal);\n"
    "   vec3 Color = texture(tCubeMap, vec2(0.0, 1.0) - UV).rgb;\n"
    "   gAlbedo = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "   gNormal = Normal;\n"
    "   gPosition = Normal * 1024.0;\n"
    "   gEmission = Color;\n"
    "}\n";

CubeMapShader::CubeMapShader() : RawShader(internalVertexShader, internalFragmentShader)
{
    build();
    locTCubeMap = getUniformLocation("tCubeMap");
}

void CubeMapShader::setCubeMap(Texture *texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getGLTextureId());
    glUniform1i(locTCubeMap, 0);
}