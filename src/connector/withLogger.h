// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "controller/logController.h"

class WithLogger
{
public:
    EXPORT WithLogger();
    static void setLogController(LogController *logController);

protected:
    LogController *logger;

private:
    static LogController *logController;
};
