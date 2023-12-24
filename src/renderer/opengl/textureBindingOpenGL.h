#pragma once
#include "renderer/textureBinding.h"

class TextureBindingOpenGL : public TextureBinding
{
public:
    void setTexture(Texture *texture) override;
    inline void setTextureId(unsigned int textureId) { this->textureId = textureId; }

    int locTexture = -1;
    unsigned int textureId = 0;
};