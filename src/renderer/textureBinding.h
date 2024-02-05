#pragma once
#include "renderer/texture.h"

class TextureBinding
{
public:
    EXPORT void setTexture(Texture *texture);
    EXPORT Texture *getTexture();

protected:
    virtual void onTextureChange();
    Texture *texture;
};