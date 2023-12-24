// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/renderer.h"
#include "connector/withLogger.h"
#include "connector/withDebug.h"
#include "connector/withCore.h"

class RendererOpenGL : public Renderer, public WithLogger, public WithDebug, public WithCore
{
public:
    EXPORT RendererOpenGL(Config *config);

    EXPORT static bool isAvailable();

    // called before window creation
    EXPORT void preInit() final override;

    // called after window creation
    EXPORT bool init(void *window) final override;

    EXPORT void presentToScreen(RenderTarget *renderTarget) final override;

    EXPORT Texture *createTexture(int width, int height, int bytesPerPixel, const void *data, bool bCreateMipmaps) final override;
    EXPORT void destroyTexture(Texture *texture) override;

    EXPORT unsigned int getWindowFlags() final override;

    EXPORT void render(RenderTarget *renderTarget) final override;

    EXPORT void renderDebugLine(Vector3 a, Vector3 b, Matrix4 *mProjectionView, float thickness, Vector3 color) final override;
    EXPORT void renderDebugLine(Matrix4 *mModel, Matrix4 *mProjectionView, Vector3 color) final override;

    EXPORT Shader *getDefaultSpriteShader() override;
    EXPORT Shader *getDefaultFramedSpriteShader() override;
    EXPORT virtual Shader *getDefaultCubeMapShader() override;

    EXPORT MeshStatic *getDefaultSpriteMesh() override;
    EXPORT virtual MeshStatic *getDefaultCubeMesh() override;

    EXPORT PhongShader *createPhongShader() override;

    EXPORT Shader *createOpenGLShader(const std::string &fragmentCode) override;
    EXPORT Shader *createOpenGLShader(const std::string &vertexCode, const std::string &fragmentCode) override;

    EXPORT EffectBuffer *createEffectBuffer() override;
    EXPORT void destroyEffectBuffer(EffectBuffer *effectBuffer) override;

    EXPORT inline std::string getOglVersion() { return oglVersion; }
    EXPORT inline std::string getVersion() { return version; }

protected:
    void setupShaderParameters(ShaderParameter **parameters, int amount);

    void renderSun(Vector3 &direction, Vector3 &colore);
    void renderSunWithShadows(RenderTarget *renderTarget, Vector3 &direction, Vector3 &color, float affectDistance);
    void renderOmni(Vector3 &position, Vector3 &color, float affectDistance);
    void renderOmniWithShadows(RenderTarget *renderTarget, Vector3 &position, Vector3 &color, float affectDistance);

    std::string oglVersion;
    std::string version;
};