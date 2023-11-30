// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "mesh/mesh.h"
#include "mesh/meshCompound.h"
#include "mesh/meshStatic.h"
#include "connector/withLogger.h"
#include "resource/resource.h"
#include "loaders3d/loader3d.h"
#include <string>

class ResourceMesh : public WithLogger, public Resource
{
public:
    EXPORT ResourceMesh(std::string meshPath);
    EXPORT ~ResourceMesh();

    EXPORT MeshCompound *getAsMeshCompound();
    EXPORT MeshStatic *getAsMeshStatic();

    EXPORT Geometry *getGeometry();

    inline void preload() { getAsMeshCompound(); }

protected:
    MeshCompound *meshCompound = nullptr;
};
