// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "resource/resourceImage.h"
#include "resource/resourceSound.h"
#include "resource/resourceFont.h"
#include "resource/resourceMesh.h"
#include <vector>

class ResourceController
{
public:
    ResourceController();

    EXPORT ResourceImage *addImage(std::string path, ByteMap byteMap = ByteMap::None, int byteMapScale = 1);
    EXPORT ResourceImage *getImageByPath(std::string path);

    EXPORT ResourceSound *addSound(std::string path);
    EXPORT ResourceSound *getSoundByPath(std::string path);

    EXPORT ResourceFont *addFont(int size);
    EXPORT ResourceFont *addFont(std::string path, int size);
    EXPORT ResourceFont *getFontByPath(std::string path, int size);

    EXPORT ResourceMesh *addMesh(std::string path);
    EXPORT ResourceMesh *getMeshByPath(std::string path);

    EXPORT void loadAll();

protected:
    std::vector<ResourceImage *> images;
    std::vector<ResourceSound *> sounds;
    std::vector<ResourceFont *> fonts;
    std::vector<ResourceMesh *> meshes;
};
