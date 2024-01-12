// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"

class Renderer;

class WithRenderer
{
public:
    static void setCurrentRenderer(Renderer *renderer);

protected:
    EXPORT static Renderer *getRenderer();

private:
    static Renderer *rendererStatic;
};
