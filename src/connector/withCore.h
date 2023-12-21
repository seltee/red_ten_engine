// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "core/core.h"

class WithCore
{
public:
    EXPORT WithCore();
    static void setGlobalCore(Core *coreGlobal);

protected:
    Core *core;

protected:
    static Core *coreGlobal;
};
