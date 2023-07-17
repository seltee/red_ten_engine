// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "os/view.h"

class Layer
{
protected:
    Layer(std::string name, int index);
    int index = 0;
    std::string name;

public:
    virtual void process(float delta);
    virtual void render(View *view);

    int getIndex();
};
