// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/renderer.h"
#include "renderer/opengl/meshStaticOpenGL.h"

Renderer::Renderer(Config *config)
{
    renderQueue = new RenderQueue();
    this->config = config;
}

Renderer::~Renderer()
{
}

void Renderer::preInit()
{
}

bool Renderer::init(void *window)
{
    return false;
}

void Renderer::presentToScreen(RenderTarget *renderTarget)
{
}

MeshStatic *Renderer::createStaticMesh()
{
    return new MeshStaticOpenGL();
}

unsigned int Renderer::getWindowFlags()
{
    return 0;
}

void Renderer::renderDebugLine(Vector3 a, Vector3 b, Matrix4 *mProjectionView, float thickness, Vector3 color)
{
    printf("Warning: wrong\n");
}

void Renderer::renderDebugLine(Matrix4 *model, Matrix4 *mProjectionView, Vector3 color)
{
    printf("Warning: wrong\n");
}

void Renderer::render(RenderTarget *renderTarget)
{
}

Shader *Renderer::getDefaultSpriteShader()
{
    return nullptr;
}

Shader *Renderer::getDefaultFramedSpriteShader()
{
    return nullptr;
}

Shader *Renderer::getDefaultCubeMapShader()
{
    return nullptr;
}

MeshStatic *Renderer::getDefaultSpriteMesh()
{
    return nullptr;
}

MeshStatic *Renderer::getDefaultCubeMesh()
{
    return nullptr;
}

PhongShader *Renderer::createPhongShader()
{
    return nullptr;
}

Shader *Renderer::createOpenGLShader(const std::string &fragmentCode)
{
    return nullptr;
}

Shader *Renderer::createOpenGLShader(const std::string &vertexCode, const std::string &fragmentCode)
{
    return nullptr;
}

Shader *Renderer::createVulkanShader(const std::string &fragmentCode)
{
    return nullptr;
}

Shader *Renderer::createVulkanShader(const std::string &vertexCode, const std::string &fragmentCode)
{
    return nullptr;
}

void Renderer::destroyShader(Shader *shader)
{
}

EffectBuffer *Renderer::createEffectBuffer()
{
    return nullptr;
}

void Renderer::destroyEffectBuffer(EffectBuffer *effectBuffer)
{
}