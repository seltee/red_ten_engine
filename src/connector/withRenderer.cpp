// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "connector/withRenderer.h"
#include "renderer/renderer.h"

Renderer *WithRenderer::rendererStatic = nullptr;

void WithRenderer::setCurrentRenderer(Renderer *renderer)
{
    WithRenderer::rendererStatic = renderer;
}

Renderer *WithRenderer::getRenderer()
{
    return rendererStatic;
}