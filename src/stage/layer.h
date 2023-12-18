// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "renderer/renderTarget.h"

class Layer
{
protected:
    Layer(std::string name, int index);
    int index = 0;
    std::string name;
    bool bIsVisible = true;

public:
    virtual void process(float delta);
    virtual void render(Renderer* renderer, RenderTarget *renderTarget);

    inline void setVisible(bool state) { bIsVisible = state; }
    inline bool isVisible() { return bIsVisible; }

    inline int getIndex() { return index; }
};
