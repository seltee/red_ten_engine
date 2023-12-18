// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

#include "resource/resource.h"
#include "renderer/texture.h"
#include "connector/withLogger.h"
#include "connector/withRenderer.h"

class ResourceHDR : public Resource, public WithLogger, public WithRenderer
{
public:
    EXPORT ResourceHDR(std::string path, float ldrScale = 1.0f, float ldrGamma = 1.0f);

    EXPORT Texture *getAsTexture();
    EXPORT Texture *getAsRadianceTexture();
    EXPORT void preload();

    EXPORT int getWidth();
    EXPORT int getHeight();

protected:
    Texture *ambientTexture = nullptr;
    Texture *redianceTexture = nullptr;

    int width = 0, height = 0, nrChannels = 0;
    float ldrScale, ldrGamma;
};