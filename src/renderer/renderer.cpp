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

Texture *Renderer::createTexture(int width, int height, int bytesPerPixel, const void *data)
{
    return nullptr;
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

Shader *Renderer::createOpenGLShader(const char *fragmentCode)
{
    return nullptr;
}

Shader *Renderer::createOpenGLShader(const char *vertexCode, const char *fragmentCode)
{
    return nullptr;
}

Shader *Renderer::createVulkanShader(const char *fragmentCode)
{
    return nullptr;
}

Shader *Renderer::createVulkanShader(const char *vertexCode, const char *fragmentCode)
{
    return nullptr;
}

EffectBuffer *Renderer::createEffectBuffer()
{
    return nullptr;
}

void Renderer::destroyEffectBuffer(EffectBuffer *effectBuffer)
{
}