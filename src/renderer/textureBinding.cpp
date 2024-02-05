#include "renderer/textureBinding.h"

void TextureBinding::setTexture(Texture *texture)
{
    this->texture = texture;
    onTextureChange();
}

Texture *TextureBinding::getTexture()
{
    return texture;
}

void TextureBinding::onTextureChange()
{
}