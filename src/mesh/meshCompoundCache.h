// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include <vector>
#include "math/math.h"

class MeshCompoundNode;

struct meshCompoundCacheEntry
{
    MeshCompoundNode *node;
    Matrix4 model;
};

class MeshCompoundCache
{
public:
    inline void addEntry(MeshCompoundNode *node, Matrix4 &model) { entriesToRender.push_back({node, model}); }
    inline void clear() { entriesToRender.clear(); }

    std::vector<meshCompoundCacheEntry> entriesToRender;
};