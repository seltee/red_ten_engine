// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "connector/withCore.h"

Core *WithCore::coreGlobal = nullptr;

WithCore::WithCore()
{
    this->core = WithCore::coreGlobal;
}

void WithCore::setGlobalCore(Core *coreGlobal)
{
    WithCore::coreGlobal = coreGlobal;
}
