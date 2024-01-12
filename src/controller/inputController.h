// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/pawn.h"
#include "os/input.h"
#include "actor/actor.h"
#include <vector>

#define MAX_CONTROLLERS 8

struct Controller
{
    void *internal;
    int id;
};

class InputController
{
public:
    InputController();

    EXPORT void provideInput(InputType type, int deviceIndex, int scancode, float value);
    EXPORT void provideInputText(std::string text);

    EXPORT bool deviceRumble(int deviceIndex, unsigned short lowFreq, unsigned short highFreq, unsigned int durationMS);
    EXPORT bool deviceRumble(unsigned short lowFreq, unsigned short highFreq, unsigned int durationMS);

    EXPORT Controller *getController(int index);

protected:
    Controller controllers[MAX_CONTROLLERS];
};
