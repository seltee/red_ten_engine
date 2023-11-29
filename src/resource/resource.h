// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <string>

class Resource
{
protected:
    Resource(std::string path);
    std::string path;

public:
    bool isPath(std::string path);
    std::string getPath();
};