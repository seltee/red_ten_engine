// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/textureOpenGL.h"
#include "renderer/opengl/glew.h"
#include <algorithm>

TextureOpengGL::TextureOpengGL(unsigned int textureID)
{
    this->textureID = textureID;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    setFiltering(this->filter);
}

TextureOpengGL::TextureOpengGL(int width, int height)
{
    printf("CREATE %i %i\n", width, height);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char *data = new unsigned char[width * height * 4];
    for (int i = 0; i < width * height; i++)
    {
        data[(i << 2)] = 64;
        data[(i << 2) + 1] = 64;
        data[(i << 2) + 2] = 64;
        data[(i << 2) + 3] = 255;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    delete[] data;

    this->width = width;
    this->height = height;

    setFiltering(this->filter);
}

TextureOpengGL::~TextureOpengGL()
{
    if (glIsTexture(textureID))
    {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

void TextureOpengGL::bind()
{
    glActiveTexture((unsigned int)TextureSlot::TEXTURE_0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureOpengGL::bind(TextureSlot slot)
{
    glActiveTexture((unsigned int)slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

unsigned int TextureOpengGL::getGLTextureId()
{
    return textureID;
}

void TextureOpengGL::setFiltering(TextureFilter filter)
{
    this->filter = filter;

    glBindTexture(GL_TEXTURE_2D, textureID);
    if (filter == TextureFilter::Nearest)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

Texture *TextureOpengGL::clone()
{
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glActiveTexture(GL_TEXTURE0);
    unsigned int newTexture;

    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, newTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, getWidth(), getHeight(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &gBuffer);

    return new TextureOpengGL(newTexture);
}