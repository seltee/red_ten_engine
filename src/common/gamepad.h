// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

enum class GamepadButtonCode
{
    UNKNOWN = -1,
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    BACK = 4,
    GUIDE = 5,
    START = 6,
    LEFT_STICK = 7,
    RIGHT_STICK = 8,
    LEFT_SHOULDER = 9,
    RIGHT_SHOULDER = 10,
    DPAD_UP = 11,
    DPAD_DOWN = 12,
    DPAD_LEFT = 13,
    DPAD_RIGHT = 14,
    MISC1 = 15,   /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button */
    PADDLE1 = 16, /* Xbox Elite paddle P1 */
    PADDLE2 = 17, /* Xbox Elite paddle P3 */
    PADDLE3 = 18, /* Xbox Elite paddle P2 */
    PADDLE4 = 19, /* Xbox Elite paddle P4 */
    TOUCHPAD = 20 /* PS4/PS5 touchpad button */
};

enum class GamepadAxisCode
{
    UNKNOWN = -1,
    LEFT_AXIS_X = 0,
    LEFT_AXIS_Y = 1,
    RIGHT_AXIS_X = 2,
    RIGHT_AXIS_Y = 3,
    LEFT_TRIGGER = 4,
    RIGHT_TRIGGER = 5
};