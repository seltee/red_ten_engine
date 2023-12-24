// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "rendererOpenGL.h"
#include "renderer/renderTarget.h"
#include "renderer/opengl/glew.h"
#include "renderer/opengl/textureOpenGL.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/opengl/shaders/phongOpenGLShader.h"
#include "renderer/opengl/shaderParameterOpenGL.h"
#include "renderer/opengl/effectBufferOpenGL.h"
#include "common/commonTextures.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "actor/actor.h"
#include <SDL.h>
#include <algorithm>

RendererOpenGL::RendererOpenGL(Config *config) : Renderer(config)
{
}

bool RendererOpenGL::isAvailable()
{
    return true;
}

void RendererOpenGL::preInit()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
}

bool RendererOpenGL::init(void *window)
{
    logger->logff("Initializing OpenGL ...");
    SDL_GLContext context = SDL_GL_CreateContext((SDL_Window *)window);
    if (!context)
    {
        logger->logff("Failed to create OpenGL context");
        return false;
    }

    glewExperimental = GL_TRUE;
    glewInit();

    CommonOpenGLShaders commonShaders;
    commonShaders.build(this);

    CommonTextures commonTextures;
    commonTextures.build(this);

    oglVersion = (char *)glGetString(GL_RENDERER); // get renderer string
    version = (char *)glGetString(GL_VERSION);     // version as a string

    logger->logff("Opengl initialized", oglVersion.c_str(), version.c_str());
    logger->logff("Renderer: %s", oglVersion.c_str() ? oglVersion.c_str() : "no renderer");
    logger->logff("OpenGL version supported: %s\n", version.c_str() ? version.c_str() : "no version");

    return true;
}

void RendererOpenGL::presentToScreen(RenderTarget *renderTarget)
{
    Matrix4 m;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    auto screenShader = CommonOpenGLShaders::getScreenShader();
    screenShader->use(m, m);

    glBindTexture(GL_TEXTURE_2D, renderTarget->getResultTexture());

    CommonOpenGLShaders::getScreenMesh()->useVertexArray();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Texture *RendererOpenGL::createTexture(int width, int height, int bytesPerPixel, const void *data, bool bCreateMipmaps)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int pixelType = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, pixelType, width, height, 0, pixelType, GL_UNSIGNED_BYTE, data);
    if (bCreateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    return new TextureOpengGL(textureID);
}

void RendererOpenGL::destroyTexture(Texture *texture)
{
    delete texture;
}

unsigned int RendererOpenGL::getWindowFlags()
{
    return SDL_WINDOW_OPENGL;
}

void RendererOpenGL::render(RenderTarget *renderTarget)
{
    Matrix4 m1, m2;
    RenderElement **elements;
    int amount;

    Matrix4 mViewProjection = *renderQueue->getViewProjectionMatrix();

    // Common parameters
    Vector3 ambientLight = renderQueue->getAmbientLight();
    Vector3 cameraPosition = renderQueue->getCameraPosition();
    bool useCameraDirectionForLights = renderQueue->isUsingCameraDirectionForLights();
    Vector3 cameraDirection = useCameraDirectionForLights ? renderQueue->getCameraDirection() : Vector3(0.0f, 0.0f, 0.0f);

    TextureOpengGL *HDRRadianceTexture =
        reinterpret_cast<TextureOpengGL *>(renderQueue->getHDRRadianceTexture() ? renderQueue->getHDRRadianceTexture() : CommonTextures::getBlackTexture());
    TextureOpengGL *HDRTexture =
        reinterpret_cast<TextureOpengGL *>(renderQueue->getHDRTexture() ? renderQueue->getHDRTexture() : CommonTextures::getBlackTexture());

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    renderTarget->setupLightning();
    renderTarget->setupNewFrame();

    // === Cube Map ===
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    if (renderQueue->isShowingEnvHDR())
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        Transformation t;
        t.setPosition(cameraPosition);
        t.setScale(Vector3(16.0f, 16.0f, 16.0f));
        t.setRotation(Vector3(0.0f, renderQueue->getEnvHDRRotation(), 0.0f));

        auto cubeMapShader = CommonOpenGLShaders::getCubeMapShader();
        auto cubeMesh = CommonOpenGLShaders::getCubeMesh();

        Matrix4 mCubeModelViewProjection = mViewProjection * *t.getModelMatrix();

        cubeMapShader->use(*t.getModelMatrix(), mCubeModelViewProjection);

        cubeMapShader->setCubeMap(renderQueue->getHDRTexture());

        cubeMesh->render();
    }

    // === Main phase ===
    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    if (renderQueue->isUsingSorting())
        glDisable(GL_DEPTH_TEST);
    else
        glEnable(GL_DEPTH_TEST);

    elements = renderQueue->getMainPhaseElements();
    int i = 0;
    do
    {
        amount = renderQueue->getMainPhaseElementsAmount();
        while (i < amount)
        {
            RenderElement *element = elements[i];
            if (element->shader && element->mesh)
            {
                element->shader->use(element->mModel, element->mModelViewProjection);

                if (element->texture)
                {
                    reinterpret_cast<TextureOpengGL *>(element->texture)->bind(TextureSlot::TEXTURE_0);
                }

                if (element->parametersAmount > 0)
                {
                    setupShaderParameters(element->parameters, element->parametersAmount);
                }

                element->mesh->render();
            }
            i++;
        }
    } while (!renderQueue->bDone || core->isBusy() || i < renderQueue->getMainPhaseElementsAmount());

    // === Initial lightning phase ===
    renderTarget->setupLightning(false);

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    auto initialLightShader = CommonOpenGLShaders::getInitialLightShader();
    initialLightShader->use(m1, m2);

    initialLightShader->setTextureAlbedo(renderTarget->getAlbedoTexture());
    initialLightShader->setTextureNormal(renderTarget->getNormalTexture());
    initialLightShader->setTexturePosition(renderTarget->getPositionTexture());
    initialLightShader->setTextureRadiance(HDRRadianceTexture->getGLTextureId());
    initialLightShader->setTextureEnvironment(HDRTexture->getGLTextureId());
    initialLightShader->setCameraPosition(cameraPosition);
    initialLightShader->setCameraDirection(cameraDirection);
    initialLightShader->setAmbientColor(glm::value_ptr(ambientLight));

    CommonOpenGLShaders::getScreenMesh()->useVertexArray();

    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // === Lightning phase ===
    RenderElementLight *lightElements = renderQueue->getLightElements();
    amount = renderQueue->getLightElementsAmount();

    for (int i = 0; i < amount; i++)
    {
        RenderElementLight *element = &lightElements[i];
        if (element->type == LightType::Sun)
        {
            Vector3 normal = glm::length2(element->position) != 0 ? glm::normalize(element->position) : Vector3(0.0f, -1.0f, 0.0f);
            if (element->bCastShadows)
                renderSunWithShadows(renderTarget, normal, element->color, element->affectDistance);
            else
                renderSun(normal, element->color);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }
        if (element->type == LightType::Omni)
        {
            if (element->bCastShadows)
                renderOmniWithShadows(renderTarget, element->position, element->color, element->affectDistance);
            else
                renderOmni(element->position, element->color, element->affectDistance);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }
    }

    // === Blending phase ===
    elements = renderQueue->getBlendingPhaseElements();
    amount = renderQueue->getBlendingPhaseElementsAmount();

    if (renderQueue->isUsingSorting())
        glDisable(GL_DEPTH_TEST);
    else
        glEnable(GL_DEPTH_TEST);

    // Sorting
    if (!renderQueue->isUsingSorting())
    {
        auto comparesort = [](const void *p, const void *q)
        { 
            Matrix4 m1 = reinterpret_cast<const RenderElement*>(p)->mModelViewProjection;
            Matrix4 m2 = reinterpret_cast<const RenderElement*>(q)->mModelViewProjection;
            Vector3 p1 = Vector3(m1 * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Vector3 p2 = Vector3(m2 * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            float len1 = p1.z;
            float len2 = p2.z;
            return len1 >= len2; };

        std::sort(elements, elements + amount, comparesort);
    }

    for (int i = 0; i < amount; i++)
    {

        RenderElement *element = elements[i];
        if (element->shader && element->mesh)
        {
            Matrix4 m = element->mModelViewProjection;
            element->shader->use(element->mModel, m);

            if (element->texture)
            {
                reinterpret_cast<TextureOpengGL *>(element->texture)->bind(TextureSlot::TEXTURE_0);
            }

            switch (element->colorMode)
            {
            case ComponentColorMode::Lit:
                glBlendFunc(GL_ONE, GL_ZERO);
                break;
            case ComponentColorMode::Alpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case ComponentColorMode::Addition:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            }

            if (element->parametersAmount > 0)
            {
                setupShaderParameters(element->parameters, element->parametersAmount);
            }

            element->shader->setOpacity(element->opacity);
            element->mesh->render();
        }
    }

    // === Debug Phase ===
    RenderElementDebug *debugElements = renderQueue->getDebugElements();
    amount = renderQueue->getDebugElementsAmount();

    glDisable(GL_DEPTH_TEST);

    for (int i = 0; i < amount; i++)
    {
        RenderElementDebug *element = &debugElements[i];
        if (element->body)
        {
            debug->renderBoundingBox(element->body->getAABB(), &mViewProjection, element->symScale, element->lineThickness, Vector3(0.2f, 0.2f, 0.2f));
            Shape *shape = element->body->getShape();
            if (shape)
            {
                shape->renderDebug(&mViewProjection, element->body->getTransformation(), 1.0f / element->symScale, element->lineThickness);
            }
        }
        if (element->actor && element->actor->isNormalsShown())
        {
            for (auto &component : *element->actor->getComponents())
            {
                auto mesh = component->getStaticMesh();
                if (mesh)
                {
                    auto vAmount = mesh->getVertexAmount();
                    auto floatsPerVertex = mesh->getFloatsPerVertex();
                    auto data = mesh->getVertexData();

                    for (int i = 0; i < vAmount; i++)
                    {
                        int shift = i * floatsPerVertex;
                        Vector4 v = Vector4(data[shift + 0], data[shift + 1], data[shift + 2], 1.0f);
                        Vector3 n = Vector3(data[shift + 3], data[shift + 4], data[shift + 5]);

                        Vector4 vts4 = *element->actor->transform.getModelMatrix() * *component->transform.getModelMatrix() * v;
                        Vector3 vts3 = Vector3(vts4.x, vts4.y, vts4.z);
                        Vector3 nr = glm::rotate(element->actor->transform.getRotation() * component->transform.getRotation(), n);

                        renderDebugLine(vts3, vts3 + nr * 0.1f, &mViewProjection, 0.01f, Vector3(0.2f, 0.9f, 0.2f));
                    }
                }
            }
        }
    }
    debug->renderAll(&mViewProjection);

    // === Final result phase ===
    renderTarget->useResultBuffer();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderOpenGL *gammaShader = (config->getAnialiasing() == AntiAliasing::FXAA) ? CommonOpenGLShaders::getGammaFXAAShader() : CommonOpenGLShaders::getGammaShader();
    gammaShader->use(m1, m2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getLightningTexture());

    float gammaEffector = 1.0f / renderQueue->getGamma();

    int gammaEffectorLoc = gammaShader->getUniformLocation("fGammaEffector");
    gammaShader->provideFloatValue(gammaEffectorLoc, 1, &gammaEffector);

    CommonOpenGLShaders::getScreenMesh()->useVertexArray();

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererOpenGL::renderDebugLine(Vector3 a, Vector3 b, Matrix4 *mProjectionView, float thickness, Vector3 color)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    auto shader = CommonOpenGLShaders::getDebugCubeShader();
    if (!shader)
        return;

    int shaderColorLoc = shader->getUniformLocation("color");

    Vector3 p = (a + b) / 2.0f;
    Vector3 dif = glm::normalize(b - a);

    float y = atan2f(dif.z, dif.x);
    float len = sqrtf(dif.x * dif.x + dif.z * dif.z);
    float x = atan2(len, dif.y);

    float lineLength = glm::length(a - b);

    Transformation tf;
    tf.setPosition(p);
    tf.setRotation(Vector3(CONST_PI / 2 - x, -y - CONST_PI / 2.0f, 0.0f));
    tf.setScale(Vector3(thickness, thickness, lineLength));
    Matrix4 *mOut = tf.getModelMatrix();

    Matrix4 mModelViewProjection = *mProjectionView * *mOut;
    shader->use(*mOut, mModelViewProjection);
    float colorFloat[3] = {color.x, color.y, color.z};
    shader->provideFloat3Value(shaderColorLoc, 1, colorFloat);

    CommonOpenGLShaders::getCubeMesh()->useVertexArray();

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RendererOpenGL::renderDebugLine(Matrix4 *mModel, Matrix4 *mProjectionView, Vector3 color)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    auto shader = CommonOpenGLShaders::getDebugCubeShader();
    if (!shader)
        return;

    int shaderColorLoc = shader->getUniformLocation("color");

    Matrix4 mModelViewProjection = *mProjectionView * *mModel;
    shader->use(*mModel, mModelViewProjection);
    float colorFloat[3] = {color.x, color.y, color.z};
    shader->provideFloat3Value(shaderColorLoc, 1, colorFloat);

    CommonOpenGLShaders::getCubeMesh()->useVertexArray();

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Shader *RendererOpenGL::getDefaultSpriteShader()
{
    return CommonOpenGLShaders::getSpriteShader();
}

Shader *RendererOpenGL::getDefaultFramedSpriteShader()
{
    return CommonOpenGLShaders::getSpriteFrameShader();
}

Shader *RendererOpenGL::getDefaultCubeMapShader()
{
    return CommonOpenGLShaders::getCubeMapShader();
}

MeshStatic *RendererOpenGL::getDefaultSpriteMesh()
{
    return CommonOpenGLShaders::getSpriteMesh();
}

MeshStatic *RendererOpenGL::getDefaultCubeMesh()
{
    return CommonOpenGLShaders::getCubeMesh();
}

PhongShader *RendererOpenGL::createPhongShader()
{
    return new PhongOpenGLShader();
}

Shader *RendererOpenGL::createOpenGLShader(const std::string &fragmentCode)
{
    std::string internalScreenVertexCode =
        "#version 410 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 texCoord;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   texCoord = aTexCoord;\n"
        "}\n";

    ShaderOpenGL *newShader = new ShaderOpenGL(internalScreenVertexCode, fragmentCode);
    newShader->build();
    return newShader;
}

Shader *RendererOpenGL::createOpenGLShader(const std::string &vertexCode, const std::string &fragmentCode)
{
    ShaderOpenGL *newShader = new ShaderOpenGL(vertexCode, fragmentCode);
    newShader->build();
    return newShader;
}

EffectBuffer *RendererOpenGL::createEffectBuffer()
{
    return new EffectBufferOpenGL();
}

void RendererOpenGL::destroyEffectBuffer(EffectBuffer *effectBuffer)
{
    if (effectBuffer)
    {
        delete effectBuffer;
    }
}

void RendererOpenGL::setupShaderParameters(ShaderParameter **parameters, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        ShaderParameterOpenGL *parameter = reinterpret_cast<ShaderParameterOpenGL *>(parameters[i]);

        switch (parameter->getType())
        {
        case ShaderParameterType::Float:
            glUniform1fv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<float *>(parameter->getData()));
            break;
        case ShaderParameterType::Float2:
            glUniform2fv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<float *>(parameter->getData()));
            break;
        case ShaderParameterType::Float3:
            glUniform3fv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<float *>(parameter->getData()));
            break;
        case ShaderParameterType::Float4:
            glUniform4fv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<float *>(parameter->getData()));
            break;
        case ShaderParameterType::Int:
            glUniform1iv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<int *>(parameter->getData()));
            break;
        case ShaderParameterType::Int2:
            glUniform2iv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<int *>(parameter->getData()));
            break;
        case ShaderParameterType::Int3:
            glUniform3iv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<int *>(parameter->getData()));
            break;
        case ShaderParameterType::Int4:
            glUniform4iv(parameter->getUniformLoc(), parameter->getAmount(), reinterpret_cast<int *>(parameter->getData()));
            break;

        default:
            break;
        }
    }
}

void RendererOpenGL::renderSun(Vector3 &direction, Vector3 &color)
{
    Vector3 cameraPosition = renderQueue->getCameraPosition();
    bool useCameraDirectionForLights = renderQueue->isUsingCameraDirectionForLights();
    Vector3 cameraDirection = useCameraDirectionForLights ? renderQueue->getCameraDirection() : Vector3(0.0f, 0.0f, 0.0f);
    auto lightShader = CommonOpenGLShaders::getSunShader();
    Matrix4 m;
    lightShader->use(m, m);

    lightShader->setLightDirection(direction);
    lightShader->setLightColor(color);
    lightShader->setCameraPosition(cameraPosition);
    lightShader->setCameraDirection(cameraDirection);

    CommonOpenGLShaders::getScreenMesh()->useVertexArray();
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererOpenGL::renderSunWithShadows(RenderTarget *renderTarget, Vector3 &direction, Vector3 &color, float affectDistance)
{
    // First render all shadow casters into shadow texture
    bool useFrontFace = renderTarget->getShadowMapSize() < 2048;
    Vector3 cameraPosition = renderQueue->getCameraPosition();
    bool useCameraDirectionForLights = renderQueue->isUsingCameraDirectionForLights();
    Vector3 cameraDirection = useCameraDirectionForLights ? renderQueue->getCameraDirection() : Vector3(0.0f, 0.0f, 0.0f);
    RenderElement **elements = renderQueue->getShadowCasterElements();
    int shadowCasterElementsAmount = renderQueue->getShadowCasterElementsAmount();
    Matrix4 m;

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    if (useFrontFace)
        glFrontFace(GL_CW);
    glDisable(GL_BLEND);

    // prepare shadowed render
    // View matrix is placement of sun
    Vector3 sunPosition = cameraPosition + direction * (affectDistance / 2.0f);
    Matrix4 rotation = glm::lookAt(sunPosition, cameraPosition, Vector3(0.0f, 1.0f, 0.0f));

    // Light is using orthogonal projection matrix
    float r = affectDistance / 2.0f;
    float l = -r;
    float b = -affectDistance / 2.0f;
    float t = -b;
    float f = affectDistance * 2.0f;
    float n = 0.0f;

    Matrix4 mLightViewProjection = glm::ortho(l, r, b, t, n, f);
    mLightViewProjection = mLightViewProjection * rotation;

    renderTarget->setupShadowHQ();

    for (int i = 0; i < shadowCasterElementsAmount; i++)
    {
        RenderElement *element = elements[i];
        if (element->mesh)
        {
            Matrix4 mModelViewProjection = mLightViewProjection * element->mModel;
            if (element->texture)
            {
                reinterpret_cast<TextureOpengGL *>(element->texture)->bind(TextureSlot::TEXTURE_0);
                CommonOpenGLShaders::getTexturedShadowShader()->use(element->mModel, mModelViewProjection);
                CommonOpenGLShaders::getTexturedShadowShader()->setUVShiftSize(element->uvShiftSize);
            }
            else
            {
                CommonOpenGLShaders::getSimpleShadowShader()->use(element->mModel, mModelViewProjection);
            }

            element->mesh->render();
        }
    }

    if (useFrontFace)
        glFrontFace(GL_CCW);

    renderTarget->setupLightning(false);

    // Render light with usage of shadow texture
    auto lightShader = CommonOpenGLShaders::getSunWithShadowShader();
    lightShader->use(m, m);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getAlbedoTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getNormalTexture());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getPositionTexture());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getShadowTexture());

    lightShader->setLightSpaceMatrix(mLightViewProjection);
    lightShader->setLightDirection(direction);
    lightShader->setLightColor(color);
    lightShader->setCameraPosition(cameraPosition);
    lightShader->setCameraDirection(cameraDirection);

    CommonOpenGLShaders::getScreenMesh()->useVertexArray();
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererOpenGL::renderOmni(Vector3 &position, Vector3 &color, float affectDistance)
{
    Matrix4 m;

    auto lightShader = CommonOpenGLShaders::getOmniShader();
    lightShader->use(m, m);

    lightShader->setAffectDistance(affectDistance);
    lightShader->setLightColor(color);
    lightShader->setLightPosition(position);

    CommonOpenGLShaders::getScreenMesh()->useVertexArray();
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererOpenGL::renderOmniWithShadows(RenderTarget *renderTarget, Vector3 &position, Vector3 &color, float affectDistance)
{
    renderOmni(position, color, affectDistance);
}