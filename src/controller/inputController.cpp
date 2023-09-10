// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "inputController.h"
#include <SDL.h>

InputController::InputController()
{
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
        controllers[i].internal = nullptr;
        controllers[i].id = -1;
    }
}

void InputController::provideInput(InputType type, int deviceIndex, int scancode, float value)
{
    auto inputs = Pawn::getInputs();
    for (auto it = inputs->begin(); it != inputs->end(); it++)
    {
        (*it)->provideInput(type, deviceIndex, scancode, value);
    }
}

bool InputController::deviceRumble(int deviceIndex, unsigned short lowFreq, unsigned short highFreq, unsigned int durationMS)
{
    int amount = SDL_NumJoysticks();

    if (deviceIndex == -1)
    {
        for (int i = 0; i < amount; i++)
        {
            auto joy = getController(i);
            bool result = false;
            if (joy)
            {
                result = SDL_JoystickRumble((SDL_Joystick *)joy->internal, lowFreq, highFreq, durationMS) == 0;
            }
            return result;
        }
    }
    else
    {
        if (deviceIndex < amount && deviceIndex >= 0)
        {
            auto joy = getController(deviceIndex);
            bool result = false;
            if (joy)
            {
                result = SDL_JoystickRumble((SDL_Joystick *)joy->internal, lowFreq, highFreq, durationMS) == 0;
            }
            return result;
        }
    }
    return false;
}

bool InputController::deviceRumble(unsigned short lowFreq, unsigned short highFreq, unsigned int durationMS)
{
    return deviceRumble(-1, lowFreq, highFreq, durationMS);
}

Controller *InputController::getController(int index)
{
    if (index < 0 || index >= MAX_CONTROLLERS)
        return nullptr;

    if (!controllers[index].internal || controllers[index].id != SDL_JoystickInstanceID((SDL_Joystick *)controllers[index].internal))
    {
        if (controllers[index].id != SDL_JoystickInstanceID((SDL_Joystick *)controllers[index].internal))
            SDL_JoystickClose((SDL_Joystick *)controllers[index].internal);

        SDL_Joystick *joy = SDL_JoystickOpen(index);
        if (joy)
        {
            controllers[index].internal = joy;
            controllers[index].id = SDL_JoystickInstanceID(joy);
        }
        else
        {
            controllers[index].internal = nullptr;
            controllers[index].id = -1;
        }
    }
    return controllers[index].internal ? &controllers[index] : nullptr;
}