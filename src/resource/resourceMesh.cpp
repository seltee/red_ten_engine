// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/resourceMesh.h"
#include "math/math.h"
#include "loaders3d/loaderFBX.h"
#include <string.h>

// number of floats in one polygon
// 3 - position
// 3 - normal
// 2 - UV
// 3 - tangent
// 3 - biTangent

ResourceMesh::ResourceMesh(std::string meshPath) : Resource(meshPath)
{
}

ResourceMesh::~ResourceMesh()
{
}

MeshCompound *ResourceMesh::getAsMeshCompound()
{
    if (meshCompound)
        return meshCompound;

    auto format = Loader3d::getFormat(path);
    Loader3d *loader = nullptr;
    switch (format)
    {
    case File3dFormat::FBX:
        loader = new LoaderFBX(path);
        break;

    default:
        loader = nullptr;
        break;
    }

    if (loader)
    {
        meshCompound = loader->getMeshCompound();
        animations = loader->getAnimations();
        delete loader;
        return meshCompound;
    }

    return nullptr;
}

MeshStatic *ResourceMesh::getAsMeshStatic()
{
    MeshCompound *mesh = getAsMeshCompound();
    if (mesh)
        return mesh->getAsStatic();
    return nullptr;
}

Geometry *ResourceMesh::getGeometry()
{
    MeshCompound *mesh = getAsMeshCompound();
    if (mesh)
        return mesh->getGeometry();
    return nullptr;
}

Animation *ResourceMesh::getFirstAnimation()
{
    return !animations.empty() ? animations.at(0) : nullptr;
}

std::vector<Animation *> ResourceMesh::getAnimations()
{
    return animations;
}

Animation *ResourceMesh::getAnimationByName(std::string name)
{
    for (auto &anim : animations)
    {
        if (anim->getName() == name)
            return anim;
    }
    return nullptr;
}