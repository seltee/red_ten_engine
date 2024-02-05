// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "loaders3d/FBX/FBXNode.h"
#include "loaders3d/FBX/FBXAnimationCurveNode.h"
#include <vector>
#include <string>

class FBXAnimationLayer
{
public:
    FBXAnimationLayer(FBXNode *node);

    void linkCurveNode(FBXAnimationCurveNode *node);

    unsigned long long id = 0;
    std::string name;

    std::vector<FBXAnimationCurveNode *> curveNodes;
};