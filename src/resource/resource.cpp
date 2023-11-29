// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/resource.h"

Resource::Resource(std::string path)
{
    this->path = path;
}

bool Resource::isPath(std::string path)
{
    return this->path == path;
}

std::string Resource::getPath()
{
    return this->path;
}