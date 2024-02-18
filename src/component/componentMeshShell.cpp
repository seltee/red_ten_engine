// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "component/componentMeshShell.h"

extern const char *shellVertexCode;
extern const char *shellFragmentCode;

Texture *ComponentMeshShell::defaultRootColor = nullptr;
Texture *ComponentMeshShell::defaultTipColor = nullptr;

const unsigned char tRootData[4] = {40, 40, 40, 255};
const unsigned char tTipData[4] = {200, 200, 200, 255};

ComponentMeshShell::ComponentMeshShell()
{
    shader = getRenderer()->createShader(shellVertexCode, shellFragmentCode);
    shellDataTextureBinding = shader->addTextureBinding("TextureData");
    shellDistorsionTextureBinding = shader->addTextureBinding("TextureDistorsion");
    shellRootTextureBinding = shader->addTextureBinding("TextureRoot");
    shellTipTextureBinding = shader->addTextureBinding("TextureTip");

    if (!defaultRootColor)
        defaultRootColor = getRenderer()->createTexture(1, 1, 4, tRootData, false);
    if (!defaultTipColor)
        defaultTipColor = getRenderer()->createTexture(1, 1, 4, tTipData, false);

    shellRootTextureBinding->setTexture(defaultRootColor);
    shellTipTextureBinding->setTexture(defaultTipColor);
}

ComponentMeshShell::~ComponentMeshShell()
{
    clearGeometry();
}

void ComponentMeshShell::onRenderQueue(RenderQueue *renderQueue)
{
    if (mesh && shader)
    {
        Matrix4 mModel = *owner->transform.getModelMatrix() * *transform.getModelMatrix();

        if (bRenderShape)
        {
            for (int i = 0; i < layersAmount; i++)
            {
                renderQueue->addMainPhase(mModel, shader, shellDataTextureBinding->getTexture(), mesh->getAsStatic(), parametersList[i], 6);
            }
        }

        // if (bCastShadows)
        //     renderQueue->addShadowCaster(mModel, mesh->getAsStatic(), shader ? shader->getShadowTexture() : nullptr, uvShadowShiftSize);
    }
}

void ComponentMeshShell::process(float delta)
{
    time += delta * distorsionSpeed;
}

void ComponentMeshShell::setMesh(Mesh *mesh)
{
    if (this->mesh != mesh)
    {
        this->mesh = mesh;
        if (owner)
            owner->childUpdated();
    }
}

void ComponentMeshShell::setDensityTexture(Texture *texture)
{
    shellDataTextureBinding->setTexture(texture);
}

void ComponentMeshShell::setDistorsionTexture(Texture *texture)
{
    shellDistorsionTextureBinding->setTexture(texture);
}

void ComponentMeshShell::setDistorsionUVShift(Vector2 shift)
{
    distorsionUVShift[0] = shift.x;
    distorsionUVShift[1] = shift.y;
}

void ComponentMeshShell::setRootTexture(Texture *texture)
{
    shellRootTextureBinding->setTexture(texture);
}

void ComponentMeshShell::setTipTexture(Texture *texture)
{
    shellTipTextureBinding->setTexture(texture);
}

void ComponentMeshShell::setParameters(int layersAmount, float layerHeight, float distorsionSpeed, float distorsionPower)
{
    this->layersAmount = layersAmount;
    this->layerHeight = layerHeight;
    this->distorsionSpeed = distorsionSpeed;
    this->distorsionPower = distorsionPower;

    recreate();
}

Matrix4 ComponentMeshShell::getLocalspaceMatrix()
{
    return *transform.getModelMatrix();
}

MeshStatic *ComponentMeshShell::getStaticMesh()
{
    return mesh->getAsStatic();
}

void ComponentMeshShell::lookAt(Vector3 &point, bool bUseGlobalTranformation)
{
    transform.setRotation(Vector3(0.0f, 0.0f, 0.0f));
    Matrix4 m = glm::inverse(bUseGlobalTranformation ? getWorldModelMatrix() : *transform.getModelMatrix());
    Vector4 localPoint = m * Vector4(point, 1.0f);
    Vector3 dif = glm::normalize(glm::vec3(localPoint));
    transform.setRotation(glm::quatLookAtRH(dif, Vector3(0.0f, 1.0f, 0.0f)) * Quat(Vector3(CONST_PI / 2.0f, CONST_PI, 0.0f)));
}

void ComponentMeshShell::clearGeometry()
{
    if (heights)
        delete[] heights;
    if (layerHeights)
        delete[] layerHeights;
    if (alphaCheck)
        delete[] alphaCheck;
    if (parametersList)
    {
        for (int i = 0; i < parametersListAmount; i++)
        {
            delete[] parametersList[i];
        }
        delete[] parametersList;
    }

    parametersList = nullptr;
    heights = nullptr;
    layerHeights = nullptr;
    alphaCheck = nullptr;
}

void ComponentMeshShell::recreate()
{
    clearGeometry();

    parametersListAmount = layersAmount;
    parametersList = new ShaderParameter **[layersAmount];
    heights = new float[layersAmount];
    layerHeights = new float[layersAmount];
    alphaCheck = new float[layersAmount];

    for (int i = 0; i < layersAmount; i++)
    {
        heights[i] = static_cast<float>(i + 1) * layerHeight;
        layerHeights[i] = static_cast<float>(i) / static_cast<float>(layersAmount);
        alphaCheck[i] = static_cast<float>(i) / static_cast<float>(layersAmount) * 0.6f + 0.1f;
        alphaCheck[i] *= alphaCheck[i];

        parametersList[i] = new ShaderParameter *[6];
        parametersList[i][0] = shader->createShaderParameter("fDist", ShaderParameterType::Float);
        parametersList[i][0]->set(1, &heights[i]);
        parametersList[i][1] = shader->createShaderParameter("fHeight", ShaderParameterType::Float);
        parametersList[i][1]->set(1, &layerHeights[i]);
        parametersList[i][2] = shader->createShaderParameter("fAlphaCheck", ShaderParameterType::Float);
        parametersList[i][2]->set(1, &alphaCheck[i]);
        parametersList[i][3] = shader->createShaderParameter("fDistorsionPower", ShaderParameterType::Float);
        parametersList[i][3]->set(1, &distorsionPower);
        parametersList[i][4] = shader->createShaderParameter("fTime", ShaderParameterType::Float);
        parametersList[i][4]->set(1, &time);
        parametersList[i][5] = shader->createShaderParameter("v2DistorsionUVShift", ShaderParameterType::Float2);
        parametersList[i][5]->set(1, distorsionUVShift);
    }
}

// Straight go shader
const char *shellVertexCode =
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
    "uniform vec2 v2DistorsionUVShift;\n"
    "uniform float fDist;\n"
    "uniform float fHeight;\n"
    "uniform float fTime;\n"
    "uniform float fDistorsionPower;\n"
    "uniform sampler2D TextureDistorsion;\n"
    "out vec2 TexCoords;\n"
    "out vec3 FragPos;\n"
    "out mat3 mTBN;\n"
    "out float ffHeight;\n"
    "void main() {\n"
    "   TexCoords = aTexCoord;\n"
    "   vec3 T = normalize((mNormal * vec4(aTangent,   0.0)).xyz);\n"
    "   vec3 B = normalize((mNormal * vec4(aBitangent, 0.0)).xyz);\n"
    "   vec3 N = normalize((mNormal * vec4(aNormal,    0.0)).xyz);\n"
    "   mTBN = mat3(T, B, N);\n"
    "   vec3 shift = mTBN * vec3(0, 0, fDist);\n"
    "   vec3 distorsion = (texture(TextureDistorsion, aTexCoord * 0.5 + vec2(fTime, fTime) + v2DistorsionUVShift).rgb * 2.0 - 1.0) * fHeight * fDistorsionPower;\n"
    "   gl_Position = mModelViewProjection * vec4(aPos + shift + distorsion, 1.0);\n"
    "   FragPos = ((mModel * vec4(aPos + shift, 1.0)).xyz) * 0.1;\n"
    "   ffHeight = fHeight;\n"
    "}\n";

const char *shellFragmentCode =
    "#version 410 core\n"
    "layout (location = 0) out vec4 gAlbedo;\n"
    "layout (location = 1) out vec4 gNormal;\n"
    "layout (location = 2) out vec3 gPosition;\n"
    "layout (location = 3) out vec4 gEmission;\n"
    "uniform sampler2D TextureData;\n"
    "uniform sampler2D TextureRoot;\n"
    "uniform sampler2D TextureTip;\n"
    "uniform float fAlphaCheck;\n"
    "in float ffHeight;\n"
    "in vec2 TexCoords;\n"
    "in vec3 FragPos;\n"
    "in mat3 mTBN;\n"
    "void main() {\n"
    "   vec4 data = texture(TextureData, TexCoords);\n"
    "   if (ffHeight >= data.r * 2.0 || data.r < 0.2) discard;\n"
    "   gAlbedo.rgba = (1 - ffHeight) * texture(TextureRoot, TexCoords) + ffHeight * texture(TextureTip, TexCoords);\n"
    "   gAlbedo.rgb *= data.b;\n"
    "   gPosition = FragPos;\n"
    "   gNormal.rgb = normalize(mTBN * vec3(0, 0, 1));\n"
    "   gNormal.a = 0.6;\n"
    "   gEmission.rgba = vec4(0, 0, 0, 0);\n"
    "}\n";
