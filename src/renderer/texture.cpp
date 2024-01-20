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