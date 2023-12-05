// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "shader.h"
#include "rawShader.h"
#include "common/texture.h"

class CubeMapShader : public RawShader
{
public:
    EXPORT CubeMapShader();
    EXPORT void setCubeMap(Texture *texture);

protected:
    unsigned int locTCubeMap = 0;

    static const char *internalVertexShader;
    static const char *internalFragmentShader;
};