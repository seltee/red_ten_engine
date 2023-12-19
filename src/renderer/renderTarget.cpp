// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/renderTarget.h"
#include "renderer/opengl/glew.h"
#include "renderer/opengl/textureOpenGL.h"

RenderTarget::RenderTarget(int width, int height, RenderQuality quality, float multiSampling)
{
    this->multiSampling = multiSampling;
    if (multiSampling == 1.0f)
    {
        this->width = width;
        this->height = height;
    }
    else
    {
        this->width = width * multiSampling;
        this->height = height * multiSampling;
    }

    resultWidth = width;
    resultHeight = height;

    switch (quality)
    {
    case RenderQuality::High:
        shadowMapSize = 4096;
        break;
    case RenderQuality::Balanced:
        shadowMapSize = 2048;
        break;

    case RenderQuality::SuperFast:
        shadowMapSize = 512;
        break;

    case RenderQuality::Fast:
    default:
        shadowMapSize = 1024;
        break;
    }

    // Rendering images
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &lightningTexture);
    glBindTexture(GL_TEXTURE_2D, lightningTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
    if (multiSampling == 1.0f)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glGenTextures(1, &resultTexture);
    glBindTexture(GL_TEXTURE_2D, resultTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resultWidth, resultHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // G BUFFERS
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // - color + specular color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // - normal color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - position color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gPosition, 0);

    // - light emission buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, lightningTexture, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);

    // Depth buffer
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Lightning buffer
    glGenFramebuffers(1, &lightningBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, lightningBuffer);

    // We draw to lightning picture only
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightningTexture, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int lightningAttachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, lightningAttachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Result buffer
    glGenFramebuffers(1, &resultBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, resultBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Shadow buffer
    glGenFramebuffers(1, &shadowBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);

    // Shadowmap
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    resultTextureAsClass = new TextureOpengGL(resultTexture);
}

RenderTarget::~RenderTarget()
{
    // Deleting buffers
    glDeleteFramebuffers(1, &gBuffer);
    glDeleteFramebuffers(1, &lightningBuffer);
    glDeleteFramebuffers(1, &shadowBuffer);
    glDeleteFramebuffers(1, &resultBuffer);

    glDeleteTextures(1, &gAlbedoSpec);
    glDeleteTextures(1, &gNormal);
    glDeleteTextures(1, &gPosition);
    glDeleteTextures(1, &lightningTexture);
    glDeleteTextures(1, &shadowTexture);
    glDeleteTextures(1, &resultTexture);
}

unsigned int RenderTarget::getPositionTexture()
{
    return gPosition;
}

unsigned int RenderTarget::getNormalTexture()
{
    return gNormal;
}

unsigned int RenderTarget::getAlbedoTexture()
{
    return gAlbedoSpec;
}

unsigned int RenderTarget::getLightningTexture()
{
    return lightningTexture;
}

unsigned int RenderTarget::getShadowTexture()
{
    return shadowTexture;
}

unsigned int RenderTarget::getResultTexture()
{
    return resultTexture;
}

void RenderTarget::useResultBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, resultBuffer);
    glViewport(0, 0, resultWidth, resultHeight);
}

void RenderTarget::setupResultBuffer(bool clear, Vector4 clearColor)
{
    glBindFramebuffer(GL_FRAMEBUFFER, resultBuffer);
    glViewport(0, 0, resultWidth, resultHeight);
    if (clear)
    {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void RenderTarget::setupNewFrame(bool clear)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glViewport(0, 0, width, height);
    if (clear)
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void RenderTarget::setupLightning(bool clear)
{
    glBindFramebuffer(GL_FRAMEBUFFER, lightningBuffer);
    glViewport(0, 0, width, height);
    if (clear)
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void RenderTarget::setupShadowHQ(bool clear)
{
    glViewport(0, 0, shadowMapSize, shadowMapSize);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    if (clear)
        glClear(GL_DEPTH_BUFFER_BIT);
}

int RenderTarget::getShadowMapSize()
{
    return shadowMapSize;
}