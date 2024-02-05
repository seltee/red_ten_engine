#pragma once
#include "renderer/textureBinding.h"

class TextureBindingOpenGL : public TextureBinding
{
protected:
    void onTextureChange() override;

public:
    int locTexture = -1;
    unsigned int textureId = 0;
};