#include "renderer/opengl/textureBindingOpenGL.h"
#include "renderer/opengl/textureOpenGL.h"

void TextureBindingOpenGL::setTexture(Texture *texture)
{
    TextureOpengGL *t = reinterpret_cast<TextureOpengGL*>(texture);
    textureId = t->getGLTextureId();
}