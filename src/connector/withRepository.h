// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "controller/resourceController.h"

class WithRepository
{
public:
    EXPORT WithRepository();
    static void setResourceController(ResourceController *resourceController);

protected:
    ResourceController *repository;

private:
    static ResourceController *resourceController;
};
