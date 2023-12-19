// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "renderer/texture.h"
#include "renderer/renderQueue.h"
#include "renderer/phongShader.h"
#include "renderer/shader.h"
#include "common/config.h"

class EffectBuffer;
class RenderTarget;

class Renderer
{
public:
    EXPORT Renderer(Config *config);
    EXPORT virtual ~Renderer();

    // called before window creation
    EXPORT virtual void preInit();

    // called after window creation
    EXPORT virtual bool init(void *window);

    EXPORT virtual void presentToScreen(RenderTarget *renderTarget);

    EXPORT virtual Texture *createTexture(int width, int height, int bytesPerPixel, const void *data);
    EXPORT virtual MeshStatic *createStaticMesh();

    EXPORT virtual void renderDebugLine(Vector3 a, Vector3 b, Matrix4 *mProjectionView, float thickness, Vector3 color);
    EXPORT virtual void renderDebugLine(Matrix4 *mModel, Matrix4 *mProjectionView, Vector3 color);

    EXPORT virtual unsigned int getWindowFlags();

    EXPORT inline RenderQueue *getRenderQueue() { return renderQueue; }

    EXPORT virtual void render(RenderTarget *renderTarget);

    EXPORT virtual Shader *getDefaultSpriteShader();
    EXPORT virtual Shader *getDefaultFramedSpriteShader();
    EXPORT virtual Shader *getDefaultCubeMapShader();
    
    EXPORT virtual MeshStatic *getDefaultSpriteMesh();
    EXPORT virtual MeshStatic *getDefaultCubeMesh();

    EXPORT virtual PhongShader *createPhongShader();

    EXPORT virtual Shader *createOpenGLShader(const char *fragmentCode);
    EXPORT virtual Shader *createOpenGLShader(const char *vertexCode, const char *fragmentCode);
    EXPORT virtual Shader *createVulkanShader(const char *fragmentCode);
    EXPORT virtual Shader *createVulkanShader(const char *vertexCode, const char *fragmentCode);

    EXPORT virtual EffectBuffer *createEffectBuffer();
    EXPORT virtual void destroyEffectBuffer(EffectBuffer *effectBuffer);

protected:
    RenderQueue *renderQueue;
    Config *config;
};