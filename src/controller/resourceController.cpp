// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resourceController.h"

ResourceController::ResourceController()
{
}

ResourceImage *ResourceController::addImage(std::string path, ByteMap byteMap, int byteMapScale)
{
    ResourceImage *image = getImageByPath(path);
    if (!image)
    {
        image = new ResourceImage(path, byteMap, byteMapScale);
        images.push_back(image);
    }
    return image;
}

ResourceImage *ResourceController::getImageByPath(std::string path)
{
    for (auto &it : images)
    {
        if (it->isPath(path))
            return it;
    }
    return nullptr;
}

ResourceSound *ResourceController::addSound(std::string path)
{
    ResourceSound *sound = getSoundByPath(path);
    if (!sound)
    {
        sound = new ResourceSound(path);
        sounds.push_back(sound);
    }
    return sound;
}

ResourceSound *ResourceController::getSoundByPath(std::string path)
{
    for (auto &it : sounds)
    {
        if (it->isPath(path))
            return it;
    }
    return nullptr;
}

ResourceFont *ResourceController::addFont(int size)
{
    ResourceFont *font = getFontByPath(std::string("default"), size);
    if (!font)
    {
        font = new ResourceFont("default", size);
        fonts.push_back(font);
    }
    return font;
}

ResourceFont *ResourceController::addFont(std::string path, int size)
{
    ResourceFont *font = getFontByPath(path, size);
    if (!font)
    {
        font = new ResourceFont(path, size);
        fonts.push_back(font);
    }
    return font;
}

ResourceFont *ResourceController::getFontByPath(std::string path, int size)
{
    for (auto &it : fonts)
    {
        if (it->isPath(path) && it->getSize() == size)
            return it;
    }
    return nullptr;
}

ResourceMesh *ResourceController::addMesh(std::string path)
{
    ResourceMesh *mesh = getMeshByPath(path);
    if (!mesh)
    {
        mesh = new ResourceMesh(path);
        meshes.push_back(mesh);
    }
    return mesh;
}

ResourceMesh *ResourceController::getMeshByPath(std::string path)
{
    for (auto &it : meshes)
    {
        if (it->isPath(path))
            return it;
    }
    return nullptr;
}

void ResourceController::loadAll()
{
    for (auto &it : images)
        it->preload();
    for (auto &it : meshes)
        it->preload();
    for (auto &it : sounds)
        it->load();
}