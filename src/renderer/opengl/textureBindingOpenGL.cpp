#include "renderer/opengl/textureBindingOpenGL.h"
#include "renderer/opengl/textureOpenGL.h"

void TextureBindingOpenGL::onTextureChange()
{
    TextureOpengGL *t = reinterpret_cast<TextureOpengGL *>(texture);
    textureId = t->getGLTextureId();
}