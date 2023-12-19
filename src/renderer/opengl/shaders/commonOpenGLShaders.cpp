// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/renderer.h"

extern float spriteData[];
extern float screenData[];
extern float cubeData[];

extern const char *spriteVertexShader;
extern const char *spriteFramedVertexShader;
extern const char *spriteFragmentShader;

extern const char *screenVertexShader;
extern const char *screenFragmentShader;

extern const char *gammaFXAAFragmentShader;
extern const char *gammaFragmentShader;

extern const char *sunFragmentCode;
extern const char *sunWithShadowFragmentCode;
extern const char *omniFragmentCode;

extern const char *debugCubeVertexCode;
extern const char *debugCubeFragmentCode;

extern const char *simpleShadowVertexShader;
extern const char *simpleShadowFragmentShader;

MeshStatic *CommonOpenGLShaders::spriteMesh = nullptr;
MeshStatic *CommonOpenGLShaders::cubeMesh = nullptr;
MeshStatic *CommonOpenGLShaders::screenMesh = nullptr;

ShaderOpenGL *CommonOpenGLShaders::spriteShader = nullptr;
ShaderOpenGL *CommonOpenGLShaders::spriteFrameShader = nullptr;
ShaderOpenGL *CommonOpenGLShaders::screenShader = nullptr;
ShaderOpenGL *CommonOpenGLShaders::gammaShader = nullptr;
ShaderOpenGL *CommonOpenGLShaders::gammaFXAAShader = nullptr;
ShaderOpenGL *CommonOpenGLShaders::effectShader = nullptr;
ShaderOpenGL *CommonOpenGLShaders::simpleShadowShader = nullptr;

InitialLightOpenGLShader *CommonOpenGLShaders::initialLightShader = nullptr;
LightningOpenGLShader *CommonOpenGLShaders::sunShader = nullptr;
LightningOpenGLShader *CommonOpenGLShaders::sunWithShadowShader = nullptr;
LightningOpenGLShader *CommonOpenGLShaders::omniShader = nullptr;

ShaderOpenGL *CommonOpenGLShaders::debugCubeShader = nullptr;

CubeMapOpenGLShader *CommonOpenGLShaders::cubeMapShader = nullptr;

void CommonOpenGLShaders::build(Renderer *renderer)
{
    logger->logff("compiling base meshes ...");

    // 3 - position, 2 - UV
    int fullAttributeSizes[2] = {3, 2};
    spriteMesh = renderer->createStaticMesh();
    spriteMesh->setupFloatsArray(spriteData, 6, 2, fullAttributeSizes);

    screenMesh = renderer->createStaticMesh();
    screenMesh->setupFloatsArray(screenData, 6, 2, fullAttributeSizes);

    // 3 - position
    int triAttributeSizes[1] = {3};
    cubeMesh = renderer->createStaticMesh();
    cubeMesh->setupFloatsArray(cubeData, 36, 1, triAttributeSizes);

    logger->logff("base meshes compiled\n");

    logger->logff("compiling shaders ...");

    logger->logff("compiling sprite shader ...");
    spriteShader = new ShaderOpenGL(spriteVertexShader, spriteFragmentShader);
    spriteShader->build();

    logger->logff("compiling framed sprite shader ...");
    spriteFrameShader = new ShaderOpenGL(spriteFramedVertexShader, spriteFragmentShader);
    spriteFrameShader->build();

    logger->logff("compiling sun shader ...");
    sunShader = new LightningOpenGLShader(screenVertexShader, sunFragmentCode);
    sunShader->build();

    logger->logff("compiling sun with shadow shader ...");
    sunWithShadowShader = new LightningOpenGLShader(screenVertexShader, sunWithShadowFragmentCode);
    sunWithShadowShader->build();

    logger->logff("compiling omni shader ...");
    omniShader = new LightningOpenGLShader(screenVertexShader, omniFragmentCode);
    sunShader->build();

    logger->logff("compiling screen shader ...");
    screenShader = new ShaderOpenGL(screenVertexShader, screenFragmentShader);
    screenShader->build();

    logger->logff("compiling effect shader ...");
    effectShader = new ShaderOpenGL(screenVertexShader, spriteFragmentShader);
    effectShader->build();

    logger->logff("compiling initial lightning shader ...");
    initialLightShader = new InitialLightOpenGLShader();

    logger->logff("compiling debug cube shader ...");
    debugCubeShader = new ShaderOpenGL(debugCubeVertexCode, debugCubeFragmentCode);
    debugCubeShader->build();

    logger->logff("compiling cube map shader ...");
    cubeMapShader = new CubeMapOpenGLShader();

    logger->logff("compiling gamma shaders ...");
    gammaShader = new ShaderOpenGL(screenVertexShader, gammaFragmentShader);
    gammaFXAAShader = new ShaderOpenGL(screenVertexShader, gammaFXAAFragmentShader);

    logger->logff("compiling simple shadow shader ...");
    simpleShadowShader = new ShaderOpenGL(simpleShadowVertexShader, simpleShadowFragmentShader);

    logger->logff("shaders compiled\n");
}

ShaderOpenGL *CommonOpenGLShaders::getSpriteShader()
{
    return spriteShader;
}

ShaderOpenGL *CommonOpenGLShaders::getSpriteFrameShader()
{
    return spriteFrameShader;
}

ShaderOpenGL *CommonOpenGLShaders::getScreenShader()
{
    return screenShader;
}

ShaderOpenGL *CommonOpenGLShaders::getEffectShader()
{
    return effectShader;
}

InitialLightOpenGLShader *CommonOpenGLShaders::getInitialLightShader()
{
    return initialLightShader;
}

ShaderOpenGL *CommonOpenGLShaders::getGammaShader()
{
    return gammaShader;
}

ShaderOpenGL *CommonOpenGLShaders::getGammaFXAAShader()
{
    return gammaFXAAShader;
}

ShaderOpenGL *CommonOpenGLShaders::getSimpleShadowShader()
{
    return simpleShadowShader;
}

LightningOpenGLShader *CommonOpenGLShaders::getSunShader()
{
    return sunShader;
}

LightningOpenGLShader *CommonOpenGLShaders::getSunWithShadowShader()
{
    return sunWithShadowShader;
}

LightningOpenGLShader *CommonOpenGLShaders::getOmniShader()
{
    return omniShader;
}

ShaderOpenGL *CommonOpenGLShaders::getDebugCubeShader()
{
    return debugCubeShader;
}

CubeMapOpenGLShader *CommonOpenGLShaders::getCubeMapShader()
{
    return cubeMapShader;
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
    "uniform mat4 mModelViewProjection;\n"
    "void main() {\n"
    "   gl_Position = mModelViewProjection * vec4(aPos, 1.0);\n"
    "   texCoord = aTexCoord;\n"
    "}\n";

const char *spriteFramedVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "uniform mat4 mModelViewProjection;\n"
    "uniform vec2 aTexCoordShift;\n"
    "uniform vec2 aTexCoordMul;\n"
    "void main() {\n"
    "   gl_Position = mModelViewProjection * vec4(aPos, 1.0);\n"
    "   texCoord = vec2(aTexCoord.x * aTexCoordMul.x + aTexCoordShift.x, aTexCoord.y * aTexCoordMul.y + aTexCoordShift.y);\n"
    "}\n";

const char *spriteFragmentShader =
    "#version 410 core\n"
    "layout (location = 0) out vec4 gAlbedoSpec;\n"
    "layout (location = 1) out vec4 gNormal;\n"
    "layout (location = 2) out vec3 gPosition;\n"
    "layout (location = 3) out vec3 gEmission;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D t;\n"
    "uniform float fOpacity;\n"
    "void main() {\n"
    "   gAlbedoSpec = texture(t, texCoord);\n"
    "   gAlbedoSpec.a *= fOpacity;\n"
    "   gNormal = vec4(0.0, 0.0, -1.0, 1.0);\n"
    "   gPosition = vec3(0.0, 0.0, 0.0);\n"
    "   gEmission = vec3(0.0, 0.0, 0.0);\n"
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

const char *gammaFXAAFragmentShader =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D t;\n"
    "uniform float fGammaEffector;\n"
    "vec2 size = textureSize(t, 0);\n"
    "#define FXAA_EDGE_THRESHOLD      (1.0/8.0)\n"
    "#define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)\n"
    "#define FXAA_SEARCH_STEPS        32\n"
    "#define FXAA_SEARCH_THRESHOLD    (1.0/4.0)\n"
    "#define FXAA_SUBPIX_CAP          (3.0/4.0)\n"
    "#define FXAA_SUBPIX_TRIM         (1.0/4.0)\n"
    "#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))\n"
    "float FxaaLuma(vec3 rgb) {\n"
    "    return rgb.y * (0.587/0.299) + rgb.x;\n"
    "}\n"
    "vec3 FxaaLerp3(vec3 a, vec3 b, float amountOfA) {\n"
    "    return (vec3(-amountOfA) * b) + ((a * vec3(amountOfA)) + b);\n"
    "}\n"
    "vec4 FxaaTexOff(sampler2D tex, vec2 pos, ivec2 off, vec2 rcpFrame) {\n"
    "    float x = pos.x + float(off.x) * rcpFrame.x;\n"
    "    float y = pos.y + float(off.y) * rcpFrame.y;\n"
    "    return texture(tex, vec2(x, y));\n"
    "}\n"
    "vec3 FxaaPixelShader(vec2 pos, sampler2D tex, vec2 rcpFrame)\n"
    "{\n"
    "    vec3 rgbN = FxaaTexOff(tex, pos.xy, ivec2( 0,-1), rcpFrame).xyz;\n"
    "    vec3 rgbW = FxaaTexOff(tex, pos.xy, ivec2(-1, 0), rcpFrame).xyz;\n"
    "    vec3 rgbM = FxaaTexOff(tex, pos.xy, ivec2( 0, 0), rcpFrame).xyz;\n"
    "    vec3 rgbE = FxaaTexOff(tex, pos.xy, ivec2( 1, 0), rcpFrame).xyz;\n"
    "    vec3 rgbS = FxaaTexOff(tex, pos.xy, ivec2( 0, 1), rcpFrame).xyz;\n"
    "    float lumaN = FxaaLuma(rgbN);\n"
    "    float lumaW = FxaaLuma(rgbW);\n"
    "    float lumaM = FxaaLuma(rgbM);\n"
    "    float lumaE = FxaaLuma(rgbE);\n"
    "    float lumaS = FxaaLuma(rgbS);\n"
    "    float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));\n"
    "    float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));\n"
    "    float range = rangeMax - rangeMin;\n"
    "    if(range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))\n"
    "    {\n"
    "        return rgbM;\n"
    "    }\n"
    "    vec3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;\n"
    "    float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;\n"
    "    float rangeL = abs(lumaL - lumaM);\n"
    "    float blendL = max(0.0, (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE; \n"
    "    blendL = min(FXAA_SUBPIX_CAP, blendL);\n"
    "    vec3 rgbNW = FxaaTexOff(tex, pos.xy, ivec2(-1,-1), rcpFrame).xyz;\n"
    "    vec3 rgbNE = FxaaTexOff(tex, pos.xy, ivec2( 1,-1), rcpFrame).xyz;\n"
    "    vec3 rgbSW = FxaaTexOff(tex, pos.xy, ivec2(-1, 1), rcpFrame).xyz;\n"
    "    vec3 rgbSE = FxaaTexOff(tex, pos.xy, ivec2( 1, 1), rcpFrame).xyz;\n"
    "    rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);\n"
    "    rgbL *= vec3(1.0/9.0);\n"
    "    float lumaNW = FxaaLuma(rgbNW);\n"
    "    float lumaNE = FxaaLuma(rgbNE);\n"
    "    float lumaSW = FxaaLuma(rgbSW);\n"
    "    float lumaSE = FxaaLuma(rgbSE);\n"
    "    float edgeVert = \n"
    "        abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +\n"
    "        abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +\n"
    "        abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));\n"
    "    float edgeHorz = \n"
    "        abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +\n"
    "        abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +\n"
    "        abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));\n"
    "        \n"
    "    bool horzSpan = edgeHorz >= edgeVert;\n"
    "    float lengthSign = horzSpan ? -rcpFrame.y : -rcpFrame.x;\n"
    "    if(!horzSpan)\n"
    "    {\n"
    "        lumaN = lumaW;\n"
    "        lumaS = lumaE;\n"
    "    }\n"
    "    float gradientN = abs(lumaN - lumaM);\n"
    "    float gradientS = abs(lumaS - lumaM);\n"
    "    lumaN = (lumaN + lumaM) * 0.5;\n"
    "    lumaS = (lumaS + lumaM) * 0.5;\n"
    "    if (gradientN < gradientS)\n"
    "    {\n"
    "        lumaN = lumaS;\n"
    "        lumaN = lumaS;\n"
    "        gradientN = gradientS;\n"
    "        lengthSign *= -1.0;\n"
    "    }\n"
    "    vec2 posN;\n"
    "    posN.x = pos.x + (horzSpan ? 0.0 : lengthSign * 0.5);\n"
    "    posN.y = pos.y + (horzSpan ? lengthSign * 0.5 : 0.0);\n"
    "    gradientN *= FXAA_SEARCH_THRESHOLD;\n"
    "    vec2 posP = posN;\n"
    "    vec2 offNP = horzSpan ? vec2(rcpFrame.x, 0.0) : vec2(0.0, rcpFrame.y); \n"
    "    float lumaEndN = lumaN;\n"
    "    float lumaEndP = lumaN;\n"
    "    bool doneN = false;\n"
    "    bool doneP = false;\n"
    "    posN += offNP * vec2(-1.0, -1.0);\n"
    "    posP += offNP * vec2( 1.0,  1.0);\n"
    "    for(int i = 0; i < FXAA_SEARCH_STEPS; i++) {\n"
    "        if(!doneN)\n"
    "        {\n"
    "            lumaEndN = FxaaLuma(texture(tex, posN.xy).xyz);\n"
    "        }\n"
    "        if(!doneP)\n"
    "        {\n"
    "            lumaEndP = FxaaLuma(texture(tex, posP.xy).xyz);\n"
    "        }\n"
    "        doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);\n"
    "        doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);\n"
    "        if(doneN && doneP)\n"
    "        {\n"
    "            break;\n"
    "        }\n"
    "        if(!doneN)\n"
    "        {\n"
    "            posN -= offNP;\n"
    "        }\n"
    "        if(!doneP)\n"
    "        {\n"
    "            posP += offNP;\n"
    "        }\n"
    "    }\n"
    "    float dstN = horzSpan ? pos.x - posN.x : pos.y - posN.y;\n"
    "    float dstP = horzSpan ? posP.x - pos.x : posP.y - pos.y;\n"
    "    bool directionN = dstN < dstP;\n"
    "    lumaEndN = directionN ? lumaEndN : lumaEndP;\n"
    "    if(((lumaM - lumaN) < 0.0) == ((lumaEndN - lumaN) < 0.0))\n"
    "    {\n"
    "        lengthSign = 0.0;\n"
    "    }\n"
    "    float spanLength = (dstP + dstN);\n"
    "    dstN = directionN ? dstN : dstP;\n"
    "    float subPixelOffset = (0.5 + (dstN * (-1.0/spanLength))) * lengthSign;\n"
    "    vec3 rgbF = texture(tex, vec2(\n"
    "        pos.x + (horzSpan ? 0.0 : subPixelOffset),\n"
    "        pos.y + (horzSpan ? subPixelOffset : 0.0))).xyz;\n"
    "    return FxaaLerp3(rgbL, rgbF, blendL); \n"
    "}\n"
    "void main() {\n"
    "   fragColor = vec4(FxaaPixelShader(texCoord, t, vec2(1.0 / size.x, 1.0 / size.y)), texture(t, texCoord).a) * 1.0;\n"
    "   fragColor.rgb = 1.055 * pow(fragColor.rgb, vec3(fGammaEffector)) - vec3(0.055);\n"
    "}\n";

const char *gammaFragmentShader =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D t;\n"
    "uniform float fGammaEffector;"
    "void main() {\n"
    "   fragColor = texture(t, texCoord);\n"
    "   fragColor.rgb = 1.055 * pow(fragColor.rgb, vec3(fGammaEffector)) - vec3(0.055);\n"
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
    "uniform vec3 cameraPos;\n"
    ""
    "   vec2 texelSize = 1.0 / textureSize(tShadowMap, 0);\n"
    "   const float PI = 3.14159265359;\n"
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
    ""
    "vec3 fresnelSchlick(float cosTheta, vec3 F0)\n"
    "{\n"
    "   return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);\n"
    "}\n"
    ""
    "float DistributionGGX(vec3 N, vec3 H, float roughness)\n"
    "{\n"
    "   float a      = roughness*roughness;\n"
    "   float a2     = a*a;\n"
    "   float NdotH  = max(dot(N, H), 0.0);\n"
    "   float NdotH2 = NdotH*NdotH;\n"
    "   float num   = a2;\n"
    "   float denom = (NdotH2 * (a2 - 1.0) + 1.0);\n"
    "   denom = PI * denom * denom;\n"
    "   return num / denom;\n"
    "}\n"
    ""
    "float GeometrySchlickGGX(float NdotV, float roughness)\n"
    "{\n"
    "   float r = (roughness + 1.0);\n"
    "   float k = (r*r) / 8.0;\n"
    "   float num   = NdotV;\n"
    "   float denom = NdotV * (1.0 - k) + k;\n"
    "   return num / denom;\n"
    "}\n"
    ""
    "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)\n"
    "{\n"
    "   float NdotV = max(dot(N, V), 0.0);\n"
    "   float NdotL = max(dot(N, L), 0.0);\n"
    "   float ggx2  = GeometrySchlickGGX(NdotV, roughness);\n"
    "   float ggx1  = GeometrySchlickGGX(NdotL, roughness);\n"
    "   return ggx1 * ggx2;\n"
    "}\n"
    ""
    "void main() {\n"
    "   float Metallic = 0.0;\n"
    "   vec3 FragPos = texture(tPosition, texCoord).rgb / 0.1;\n"
    "   vec4 NormalData = texture(tNormal, texCoord);\n"
    "   vec3 Normal = NormalData.rgb;\n"
    "   float Roughness = NormalData.a;\n"
    "   float s = texture(tShadowMap, texCoord).r;\n"
    "   vec3 Albedo = texture(tAlbedoSpec, texCoord).rgb;\n"
    "   vec3 L = lightDir;\n"
    "   vec3 V = normalize(cameraPos - FragPos);\n"
    "   vec3 H = normalize(V + L);\n"
    "   vec3 F0 = vec3(0.04);\n"
    "   F0 = mix(F0, Albedo, Metallic);\n"
    "   vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);\n"
    "   float NDF = DistributionGGX(Normal, H, Roughness);\n"
    "   float G = GeometrySmith(Normal, V, L, Roughness);\n"
    "   vec3 numerator = NDF * G * F;\n"
    "   float denominator = 4.0 * max(dot(Normal, V), 0.0) * max(dot(Normal, L), 0.0) + 0.0001;\n"
    "   vec3 Specular = numerator / denominator;\n"
    "   vec3 kS = F;\n"
    "   vec3 kD = vec3(1.0) - kS;\n"
    "   kD *= 1.0 - Metallic;\n"
    "   vec4 FragPosLightSpace = mlightSpace * vec4(FragPos, 1.0);\n"
    "   float Shadow = ShadowCalculation(FragPosLightSpace, Normal);\n"
    "   float NdotL = max(dot(Normal, L), 0.0);\n"
    "   vec3 Light = (kD * Albedo / PI + Specular) * lightColor * NdotL * (1.0 - Shadow);\n"
    "   FragColor = vec4(Light, 0.0);\n"
    "}\n";
// ^ Todo
// F0 = mix(F0, Albedo, Metallic)
// Add Metallic textures
// L = lightDir, vec3 L = normalize(lightPositions[i] - WorldPos);
// vec3 N = normalize(Normal);
// vec3 V = normalize(camPos - WorldPos);
// vec3 L = normalize(lightPositions[i] - WorldPos);
// vec3 H = normalize(V + L);
// vec3 radiance = lightColors[i] * attenuation;
// attenuation - float attenuation = 1.0 / (distance * distance);, always 1 for sun
// vec3 radiance     = lightColors[i] * attenuation;  always lightColor

const char *omniFragmentCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tPosition;\n"
    "uniform sampler2D tAlbedoSpec;\n"
    "uniform sampler2D tNormal;\n"
    "uniform vec3 lightColor;\n"
    "uniform float affectDistance;\n"
    "uniform vec3 v3Position;\n"
    "void main() {\n"
    "   vec3 FragPos = texture(tPosition, texCoord).rgb;\n"
    "   vec3 Normal = texture(tNormal, texCoord).rgb;\n"
    "   vec3 Albedo = texture(tAlbedoSpec, texCoord).rgb;\n"
    "   vec3 dif = v3Position * 0.1 - FragPos;"
    "   vec3 lightDir = normalize(dif);\n"
    "   float distPower = max(1.0 - (length(dif) /  affectDistance), 0.0);\n"
    "   vec3 light = max(dot(Normal, lightDir), 0.0) * Albedo * lightColor * distPower;\n"
    "   FragColor = vec4(light, 0.0);\n"
    "}\n";

const char *debugCubeVertexCode =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 mModelViewProjection;\n"
    "void main() {\n"
    "   gl_Position = mModelViewProjection * vec4(aPos, 1.0);\n"
    "}\n";

const char *debugCubeFragmentCode =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "uniform vec3 color;\n"
    "void main() {\n"
    "   fragColor = vec4(color, 1.0);\n"
    "}\n";

const char *simpleShadowVertexShader =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 mModelViewProjection;\n"
    "void main() {\n"
    "   gl_Position = mModelViewProjection * vec4(aPos, 1.0);\n"
    "}\n";

const char *simpleShadowFragmentShader =
    "#version 410 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";