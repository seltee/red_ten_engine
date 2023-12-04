// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "common/commonShaders.h"

extern float spriteData[];
extern float screenData[];
extern float cubeData[];

extern const char *spriteVertexShader;
extern const char *spriteFramedVertexShader;
extern const char *spriteFragmentShader;

extern const char *screenVertexShader;
extern const char *screenFragmentShader;

extern const char *clearFragmentShader;

extern const char *initialLightningFragmentCode;

extern const char *sunFragmentCode;
extern const char *sunWithShadowFragmentCode;
extern const char *omniFragmentCode;

extern const char *debugCubeVertexCode;
extern const char *debugCubeFragmentCode;

MeshStatic *CommonShaders::spriteMesh = nullptr;
MeshStatic *CommonShaders::cubeMesh = nullptr;
MeshStatic *CommonShaders::screenMesh = nullptr;

Shader *CommonShaders::spriteShader = nullptr;
Shader *CommonShaders::spriteFrameShader = nullptr;
Shader *CommonShaders::screenShader = nullptr;
RawShader *CommonShaders::effectShader = nullptr;
RawShader *CommonShaders::initialLightningShader = nullptr;

LightningShader *CommonShaders::sunShader = nullptr;
LightningShader *CommonShaders::sunWithShadowShader = nullptr;
LightningShader *CommonShaders::omniShader = nullptr;

RawShader *CommonShaders::debugCubeShader = nullptr;

void CommonShaders::build()
{
    logger->logff("compiling base meshes ...");

    // 3 - position, 2 - UV
    int fullAttributeSizes[2] = {3, 2};
    spriteMesh = new MeshStatic();
    spriteMesh->setupFloatsArray(spriteData, 6, 2, fullAttributeSizes);

    screenMesh = new MeshStatic();
    screenMesh->setupFloatsArray(screenData, 6, 2, fullAttributeSizes);

    // 3 - position
    int triAttributeSizes[1] = {3};
    cubeMesh = new MeshStatic();
    cubeMesh->setupFloatsArray(cubeData, 36, 1, triAttributeSizes);

    logger->logff("base meshes compiled\n");

    logger->logff("compiling shaders ...");

    logger->logff("compiling sprite shader ...");
    spriteShader = new RawShader(spriteVertexShader, spriteFragmentShader);
    spriteShader->build();

    logger->logff("compiling framed sprite shader ...");
    spriteFrameShader = new RawShader(spriteFramedVertexShader, spriteFragmentShader);
    spriteFrameShader->build();

    logger->logff("compiling sun shader ...");
    sunShader = new LightningShader(screenVertexShader, sunFragmentCode);
    sunShader->build();

    logger->logff("compiling sun with shadow shader ...");
    sunWithShadowShader = new LightningShader(screenVertexShader, sunWithShadowFragmentCode);
    sunWithShadowShader->build();

    logger->logff("compiling omni shader ...");
    omniShader = new LightningShader(screenVertexShader, omniFragmentCode);
    sunShader->build();

    logger->logff("compiling screen shader ...");
    screenShader = new RawShader(screenVertexShader, screenFragmentShader);
    screenShader->build();

    logger->logff("compiling effect shader ...");
    effectShader = new RawShader(screenVertexShader, spriteFragmentShader);
    effectShader->build();

    logger->logff("compiling initial lightning shader ...");
    initialLightningShader = new RawShader(screenVertexShader, initialLightningFragmentCode);
    initialLightningShader->build();

    logger->logff("compiling debug cube shader ...");
    debugCubeShader = new RawShader(debugCubeVertexCode, debugCubeFragmentCode);
    debugCubeShader->build();

    logger->logff("shaders compiled\n");
}

Shader *CommonShaders::getSpriteShader()
{
    return spriteShader;
}

Shader *CommonShaders::getSpriteFrameShader()
{
    return spriteFrameShader;
}

Shader *CommonShaders::getScreenShader()
{
    return screenShader;
}

RawShader *CommonShaders::getEffectShader()
{
    return effectShader;
}

RawShader *CommonShaders::getInitialLightningShader()
{
    return initialLightningShader;
}

LightningShader *CommonShaders::getSunShader()
{
    return sunShader;
}

LightningShader *CommonShaders::getSunWithShadowShader()
{
    return sunWithShadowShader;
}

LightningShader *CommonShaders::getOmniShader()
{
    return omniShader;
}

RawShader *CommonShaders::getDebugCubeShader()
{
    return debugCubeShader;
}

float spriteData[] = {
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

float screenData[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

float cubeData[] = {
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f};

const char *spriteVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "uniform mat4 mTransform;\n"
    "uniform mat4 mViewProjection;\n"
    "void main() {\n"
    "   gl_Position = mViewProjection * mTransform * vec4(aPos, 1.0);\n"
    "   texCoord = aTexCoord;\n"
    "}\n";

const char *spriteFramedVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "uniform mat4 mTransform;\n"
    "uniform mat4 mViewProjection;\n"
    "uniform vec2 aTexCoordShift;\n"
    "uniform vec2 aTexCoordMul;\n"
    "void main() {\n"
    "   gl_Position = mViewProjection * mTransform * vec4(aPos, 1.0);\n"
    "   texCoord = vec2(aTexCoord.x * aTexCoordMul.x + aTexCoordShift.x, aTexCoord.y * aTexCoordMul.y + aTexCoordShift.y);\n"
    "}\n";

const char *spriteFragmentShader =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D t;\n"
    "uniform float fOpacity;\n"
    "void main() {\n"
    "   vec4 color = texture(t, texCoord);\n"
    "   fragColor = color * fOpacity;\n"
    "}\n";

const char *screenVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   texCoord = aTexCoord;\n"
    "}\n";

const char *screenFragmentShader =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D t;\n"
    "void main() {\n"
    "   fragColor = texture(t, texCoord);\n"
    "}\n";

const char *clearFragmentShader =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "uniform vec3 clearColor;\n"
    "void main() {\n"
    "   fragColor = vec4(clearColor, 1.0);\n"
    "}\n";

const char *initialLightningFragmentCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tAlbedo;\n"
    "uniform sampler2D tNormal;\n"
    "uniform vec3 vf3ambientColor;\n"
    "void main() {\n"
    "   vec3 Albedo = texture(tAlbedo, texCoord).rgb;\n"
    "   vec3 Normal = texture(tNormal, texCoord).rgb;\n"
    "   FragColor = length(Normal) == 0 ? vec4(0.0, 0, 0, 0.0) : vec4(Albedo * vf3ambientColor, 1.0);\n"
    "}\n";

const char *sunFragmentCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tAlbedoSpec;\n"
    "uniform sampler2D tNormal;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 lightDir;\n"
    "void main() {\n"
    "   vec3 Normal = texture(tNormal, texCoord).rgb;\n"
    "   vec3 Albedo = texture(tAlbedoSpec, texCoord).rgb;\n"
    "   vec3 light = max(dot(Normal, lightDir), 0.0) * Albedo * lightColor;\n"
    "   FragColor = vec4(light, 0.0);\n"
    "}\n";

const char *sunWithShadowFragmentCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tPosition;\n"
    "uniform sampler2D tAlbedoSpec;\n"
    "uniform sampler2D tNormal;\n"
    "uniform sampler2D tShadowMap;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 lightDir;\n"
    "uniform mat4 mlightSpace;\n"
    ""
    "vec2 texelSize = 1.0 / textureSize(tShadowMap, 0);\n"
    ""
    "float ShadowCalculation(vec4 fragPosLightSpace, vec3 Normal)\n"
    "{\n"
    "   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
    "   projCoords = projCoords * 0.5 + 0.5;\n"
    "   float currentDepth = projCoords.z;\n"
    "   float bias = max(0.002 * (1.0 - dot(Normal, lightDir)), 0.0006);\n"
    "   float shadow = 0.0;\n"
    "   for(int x = -2; x <= 2; ++x){\n"
    "       for(int y = -2; y <= 2; ++y){\n"
    "           vec2 coords = projCoords.xy + vec2(x, y) * texelSize;\n"
    "           if (coords.x >= 0.0 && coords.x < 1.0 && coords.y >= 0.0 && coords.y < 1.0){\n"
    "               float pcfDepth = texture(tShadowMap, coords).r;\n"
    "               shadow += (currentDepth - bias > pcfDepth) || currentDepth > 1.0 ? 1.0 : 0.0;\n"
    "           } else {\n"
    "               shadow += 0.0;\n"
    "           }\n"
    "       }\n"
    "   }\n"
    "   shadow /= 25.0;\n"
    "   return shadow;\n"
    "}\n"
    "void main() {\n"
    "   vec3 FragPos = texture(tPosition, texCoord).rgb / 0.1;\n"
    "   vec3 Normal = texture(tNormal, texCoord).rgb;\n"
    "   vec3 Albedo = texture(tAlbedoSpec, texCoord).rgb;\n"
    "   vec4 FragPosLightSpace = mlightSpace * vec4(FragPos, 1.0);\n"
    "   float shadow = ShadowCalculation(FragPosLightSpace, Normal);\n"
    "   vec3 light = max(dot(Normal, lightDir), 0.0) * (1.0 - shadow) * Albedo * lightColor;\n"
    "   FragColor = vec4(light, 0.0);\n"
    "}\n";

const char *omniFragmentCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tPosition;\n"
    "uniform sampler2D tAlbedoSpec;\n"
    "uniform sampler2D tNormal;\n"
    "uniform vec3 lightColor;\n"
    "uniform float affectDistance;\n"
    "uniform mat4 mTransform;\n"
    "void main() {\n"
    "   vec3 FragPos = texture(tPosition, texCoord).rgb;\n"
    "   vec3 Normal = texture(tNormal, texCoord).rgb;\n"
    "   vec3 Albedo = texture(tAlbedoSpec, texCoord).rgb;\n"
    "   vec3 position = vec3(mTransform * vec4(0.0, 0.0, 0.0, 1.0)) * 0.1;\n"
    "   vec3 dif = position - FragPos;"
    "   vec3 lightDir = normalize(dif);\n"
    "   float distPower = max(1.0 - (length(dif) /  affectDistance), 0.0);\n"
    "   vec3 light = max(dot(Normal, lightDir), 0.0) * Albedo * lightColor * distPower;\n"
    "   FragColor = vec4(light, 0.0);\n"
    "}\n";

const char *debugCubeVertexCode =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 mTransform;\n"
    "uniform mat4 mViewProjection;\n"
    "void main() {\n"
    "   gl_Position = mViewProjection * mTransform * vec4(aPos, 1.0);\n"
    "}\n";

const char *debugCubeFragmentCode =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "uniform vec3 color;\n"
    "void main() {\n"
    "   fragColor = vec4(color, 1.0);\n"
    "}\n";
