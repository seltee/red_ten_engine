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

TextureOpengGL::~TextureOpengGL()
{
    if (glIsTexture(textureID))
    {
        glDeleteTextures(1, &textureID);
        textureID = -1;
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
