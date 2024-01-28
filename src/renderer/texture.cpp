// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/texture.h"
#include <algorithm>

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::setFiltering(TextureFilter filter)
{
    this->filter = filter;
}

Texture *Texture::clone()
{
    return nullptr;
}

bool Texture::isEditable()
{
    return false;
}

void Texture::drawImage(Texture *texture, Vector2 position)
{
}

void Texture::drawImage(Texture *texture, Vector2 position, Vector2 Scale)
{
}

void Texture::drawImage(Texture *texture, Vector2 position, Vector2 Scale, Vector2 alignPoint, float rotation)
{
}

void Texture::drawImage(Texture *texture, Vector2 position, Vector2 Scale, float rotation)
{
}

void Texture::drawImage(Texture *texture, Matrix4 *model, Shader *shader)
{
}

void Texture::recreateMipMaps()
{
}