// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "withRepository.h"

ResourceController *WithRepository::resourceController = nullptr;

WithRepository::WithRepository()
{
    this->repository = WithRepository::resourceController;
}

void WithRepository::setResourceController(ResourceController *resourceController)
{
    WithRepository::resourceController = resourceController;
}
