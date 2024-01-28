// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "renderer/opengl/textureEditableOpenGL.h"
#include "renderer/opengl/glew.h"
#include "math/transformation.h"
#include "renderer/opengl/shaders/commonOpenGLShaders.h"

TextureEditableOpenGL::TextureEditableOpenGL(int width, int height) : TextureOpengGL(width, height)
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureEditableOpenGL::drawImage(Texture *texture, Vector2 position)
{
    Transformation t;
    t.setPosition((position * 2.0f - Vector2(1.0f, 1.0f)));
    t.setScale(static_cast<float>(texture->getWidth()) / static_cast<float>(width) * 2.0f, static_cast<float>(texture->getHeight()) / static_cast<float>(height) * 2.0f);
    drawImage(texture, t.getModelMatrix(), CommonOpenGLShaders::getSpriteShader());
}

void TextureEditableOpenGL::drawImage(Texture *texture, Vector2 position, Vector2 scale)
{
    Transformation t;
    t.setPosition((position * 2.0f - Vector2(1.0f, 1.0f)));
    t.setScale(static_cast<float>(texture->getWidth()) / static_cast<float>(width) * 2.0f * scale.x, static_cast<float>(texture->getHeight()) / static_cast<float>(height) * 2.0f * scale.y);
    drawImage(texture, t.getModelMatrix(), CommonOpenGLShaders::getSpriteShader());
}

void TextureEditableOpenGL::drawImage(Texture *texture, Vector2 position, Vector2 scale, float rotation)
{
    drawImage(texture, position, scale, Vector2(0), rotation);
}

void TextureEditableOpenGL::drawImage(Texture *texture, Vector2 position, Vector2 scale, Vector2 alignPoint, float rotation)
{
    Transformation t;
    t.setPosition((position * 2.0f - Vector2(1.0f, 1.0f)));
    t.setScale(static_cast<float>(texture->getWidth()) / static_cast<float>(width) * 2.0f * scale.x, static_cast<float>(texture->getHeight()) / static_cast<float>(height) * 2.0f * scale.y);
    t.setRotation(rotation);

    Matrix4 mAnchor = glm::translate(Matrix4(1.0f), Vector3(-alignPoint.x, -alignPoint.y, 0.0f));
    Matrix4 mModel = *t.getModelMatrix() * mAnchor;

    drawImage(texture, &mModel, CommonOpenGLShaders::getSpriteShader());
}

void TextureEditableOpenGL::drawImage(Texture *texture, Matrix4 *model, Shader *shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glViewport(0, 0, width, height);

    shader->use(*model, *model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reinterpret_cast<TextureOpengGL *>(texture)->getGLTextureId());
    shader->setOpacity(1.0f);

    CommonOpenGLShaders::getSpriteMesh()->useVertexArray();

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TextureEditableOpenGL::recreateMipMaps()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGenerateMipmap(GL_TEXTURE_2D);
}

bool TextureEditableOpenGL::isEditable()
{
    return true;
}