// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "withDebug.h"
#include "controller/debugController.h"

DebugController *WithDebug::debugController = nullptr;

WithDebug::WithDebug()
{
    this->debug = WithDebug::debugController;
}

void WithDebug::setDebugController(DebugController *debugController)
{
    WithDebug::debugController = debugController;
}
