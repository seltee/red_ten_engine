// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "math/glm/gtc/type_ptr.hpp"
#include "component/componentText.h"
#include "opengl/glew.h"
#include "common/commonShaders.h"
#include <math.h>
#include <SDL_ttf.h>
#include <SDL.h>

ComponentText::ComponentText()
{
    mAnchor = Matrix4(1.0f);
    setAnchor(0.5f, 0.5f);
    colorMode = ComponentColorMode::Alpha;
}

EXPORT ComponentText::~ComponentText()
{
    if (textureID != 0)
        glDeleteTextures(1, &textureID);
}

void ComponentText::onRender(Matrix4 &vpMatrix, Transformation *tf)
{
    if (isStringDirty)
        rebuildString();

    if (textureID)
    {
        Matrix4 mTransform = *transform.getModelMatrix();
        Matrix4 mModelTransform = *tf->getModelMatrix() * mTransform;

        Matrix4 mOut = mModelTransform * mAnchor;

        auto shader = CommonShaders::getSpriteShader();
        shader->use(vpMatrix, mOut);
        shader->setOpacity(opacity);

        glActiveTexture((unsigned int)TextureSlot::TEXTURE_0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        CommonShaders::getSpriteMesh()->useVertexArray();
        prepareColorMode();

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void ComponentText::onRenderShadow(Matrix4 &vpMatrix, Transformation *tf){

}

void ComponentText::setOpacity(float opacity)
{
    this->opacity = fmaxf(fminf(opacity, 1.0f), 0.0f);
}

float ComponentText::getOpacity()
{
    return opacity;
}

void ComponentText::setAnchor(float x, float y)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-x, -y, 0.0f));
}

void ComponentText::setAnchor(Vector2 &anchor)
{
    mAnchor = glm::translate(Matrix4(1.0f), Vector3(-anchor.x, -anchor.y, 0.0f));
}

void ComponentText::setFont(ResourceFont *font)
{
    if (this->font != font)
    {
        this->font = font;
        isStringDirty = true;
    }
}

void ComponentText::setColor(unsigned char r, unsigned char g, unsigned char b)
{
    if (this->r != r || this->g != g || this->b != b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        isStringDirty = true;
    }
}

void ComponentText::setText(std::string string)
{
    if (this->string != string)
    {
        this->string = string;
        isStringDirty = true;
    }
}

std::string ComponentText::getText()
{
    return string;
}

void ComponentText::rebuildString()
{
    isStringDirty = false;
    if (textureID != 0)
        glDeleteTextures(1, &textureID);
    textureID = 0;

    if (font && font->getFont() && string.length() > 0)
    {
        SDL_Color color = {b, g, r};
        SDL_Surface *surface = TTF_RenderUTF8_Blended((TTF_Font *)font->getFont(), string.c_str(), color);

        if (!surface)
        {
            logger->logff("Failed to render text into surface: %s", string.c_str());
            textTextureWidth = 0;
            textTextureHeight = 0;
        }
        else
        {
            textTextureWidth = surface->pitch / 4;
            textTextureHeight = surface->h;

            transform.setScale(textTextureWidth, textTextureHeight);

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textTextureWidth, textTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            SDL_FreeSurface(surface);
        }
    }
}

int ComponentText::getWidth()
{
    return textTextureWidth;
}

int ComponentText::getHeight()
{
    return textTextureHeight;
}

Matrix4 ComponentText::getLocalspaceMatrix()
{
    Matrix4 mTransform = *transform.getModelMatrix();
    Matrix4 mOut(1.0f);
    mOut *= mAnchor;
    mOut *= mTransform;

    return mOut;
}