// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "loaders3d/loader3d.h"

Loader3d::Loader3d(std::string path)
{
    this->path = path;
}

Loader3d::~Loader3d()
{
}

File3dFormat Loader3d::getFormat(std::string fileName)
{
    int length = fileName.length();
    if (length > 5)
    {
        if (
            (fileName[length - 1] == 'x' || fileName[length - 1] == 'X') &&
            (fileName[length - 2] == 'b' || fileName[length - 2] == 'B') &&
            (fileName[length - 3] == 'f' || fileName[length - 3] == 'F') &&
            fileName[length - 4] == '.')
        {
            return File3dFormat::FBX;
        }
    }
    return File3dFormat::Unknown;
}

MeshCompound *Loader3d::getMeshCompound()
{
    return nullptr;
}

std::vector<Animation *> Loader3d::getAnimations()
{
    std::vector<Animation *> v;
    return v;
}