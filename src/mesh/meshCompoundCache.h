// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include "math/math.h"

class MeshCompoundNode;

struct MeshCompoundCacheEntry
{
    MeshCompoundNode *node;
    Matrix4 model;
    bool isDirty;
};

class MeshCompoundCache
{
public:
    EXPORT inline MeshCompoundCacheEntry *addEntry(MeshCompoundNode *node)
    {
        entriesToRender.push_back({node, Matrix4(1.0f), true});
        return &entriesToRender.back();
    }
    EXPORT inline MeshCompoundCacheEntry *addEntry(MeshCompoundNode *node, Matrix4 &model)
    {
        entriesToRender.push_back({node, model, true});
        return &entriesToRender.back();
    }
    EXPORT inline void clear() { entriesToRender.clear(); }
    EXPORT inline void makeDirty()
    {
        for (auto &entry : entriesToRender)
            entry.isDirty = true;
    }
    EXPORT inline MeshCompoundCacheEntry *getEntryByNode(MeshCompoundNode *node)
    {
        for (auto &entry : entriesToRender)
        {
            if (entry.node == node)
                return &entry;
        }
        return nullptr;
    }

    std::vector<MeshCompoundCacheEntry> entriesToRender;
};