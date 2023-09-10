// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "withProfiler.h"

ProfilerController *WithProfiler::profilerController = nullptr;

WithProfiler::WithProfiler()
{
    this->profiler = WithProfiler::profilerController;
}

void WithProfiler::setProfilerController(ProfilerController *profilerController)
{
    WithProfiler::profilerController = profilerController;
}
