// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/effectBufferOpenGL.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"
#include "renderer/opengl/glew.h"

EffectBufferOpenGL::EffectBufferOpenGL()
{
    glGenFramebuffers(1, &framebuffer);
}

EffectBufferOpenGL::~EffectBufferOpenGL()
{
    glDeleteFramebuffers(1, &framebuffer);
}

void EffectBufferOpenGL::prepare(int width, int height)
{
    if (this->width != width || this->height != height)
    {
        this->width = width;
        this->height = height;

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void EffectBufferOpenGL::render(Renderer *renderer, RenderTarget *renderTarget, ParametredShader *effect)
{
    auto *effectShader = CommonOpenGLShaders::getEffectShader();

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    Matrix4 m(1.0f);

    effect->shader->use(m, m);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getResultTexture());
    for (int i = 0; i < effect->parametersAmount; i++)
    {
        effect->parameters[i]->apply();
    }
    CommonOpenGLShaders::getScreenMesh()->useVertexArray();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    renderTarget->useResultBuffer();
    effectShader->use(m, m);
    effectShader->setOpacity(effect->shader->getOpacity());

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    CommonOpenGLShaders::getScreenMesh()->useVertexArray();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}