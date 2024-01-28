// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "renderer/opengl/textureOpenGL.h"
#include "math/math.h"

class TextureEditableOpenGL : public TextureOpengGL
{
public:
    TextureEditableOpenGL(int width, int height);
    void drawImage(Texture *texture, Vector2 position) override final;
    void drawImage(Texture *texture, Vector2 position, Vector2 scale) override final;
    void drawImage(Texture *texture, Vector2 position, Vector2 scale, float rotation) override final;
    void drawImage(Texture *texture, Vector2 position, Vector2 scale, Vector2 alignPoint, float rotation) override final;
    void drawImage(Texture *texture, Matrix4 *model, Shader *shader) override final;
    void recreateMipMaps() override final;

    bool isEditable() override;

protected:
    unsigned int gBuffer = 0;
};