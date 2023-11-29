// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "common/texture.h"
#include "opengl/glew.h"
#include <algorithm>

Texture::Texture(unsigned int textureID)
{
    this->textureID = textureID;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    setFiltering(this->filter);
}

Texture::~Texture()
{
    if (glIsTexture(textureID))
    {
        glDeleteTextures(1, &textureID);
        textureID = -1;
    }
}

void Texture::bind()
{
    glActiveTexture((unsigned int)TextureSlot::TEXTURE_0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::bind(TextureSlot slot)
{
    glActiveTexture((unsigned int)slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

unsigned int Texture::getGLTextureId()
{
    return textureID;
}

void Texture::setFiltering(TextureFilter filter)
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

TextureFilter Texture::getFiltering()
{
    return filter;
}