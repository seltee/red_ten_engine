// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "controller/profilerController.h"

class WithProfiler
{
public:
    EXPORT WithProfiler();
    static void setProfilerController(ProfilerController *logController);

protected:
    ProfilerController *profiler;

private:
    static ProfilerController *profilerController;
};
