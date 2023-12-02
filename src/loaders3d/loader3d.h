// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "mesh/meshCompound.h"
#include "animation/animation.h"
#include <string>
#include <vector>

enum class File3dFormat
{
    Unknown,
    FBX
};

class Loader3d
{
protected:
    Loader3d(std::string path);
    std::string path;

public:
    EXPORT virtual ~Loader3d();

    static File3dFormat getFormat(std::string fileName);
    virtual MeshCompound *getMeshCompound();

    virtual std::vector<Animation *> getAnimations();
};