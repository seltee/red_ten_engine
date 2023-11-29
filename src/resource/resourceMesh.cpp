// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/resourceMesh.h"
#include "opengl/glew.h"
#include "loaders/fbx_loader.h"
#include "math/math.h"
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
    if (geometry)
        delete geometry;
}

void ResourceMesh::reload()
{
    if (path.length() > 0 && !bLoaded)
    {
        logger->logf("Add mesh %s", path.c_str());

        FBXLoader *loader = new FBXLoader();
        bool result = loader->load(path);
        if (result && loader->root)
        {
            int amountOfFloats;
            float *data = loader->root->getAsFloatArray8f(&amountOfFloats);
            if (data)
            {
                setupByArray8f(data, amountOfFloats);
                bLoaded = true;
            }
            free(data);
        }
        else
        {
            logger->logff("Failed loading %s\n", path.c_str());
        }
        delete loader;
    }
}

bool ResourceMesh::isLoaded()
{
    return bLoaded;
}

Geometry *ResourceMesh::getGeometry()
{
    reload();
    return geometry;
}

void ResourceMesh::setupByArray8f(const float *data, int amount)
{
    // calculating tangents and bitangents
    int amountOfVertexes = amount / 8;
    int attributeSizes[3] = {3, 3, 2};
    setupFloatsArray(data, amountOfVertexes, 3, attributeSizes, true);
    geometry = new Geometry(data, amountOfVertexes, 8, 0);
    bLoaded = true;
}

void ResourceMesh::use()
{
    if (!bLoaded)
        reload();

    if (vao)
        glBindVertexArray(vao);
}
