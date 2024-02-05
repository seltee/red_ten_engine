// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "FBXNode.h"
#include "FBXAnimationLayer.h"
#include <vector>

// The animation stack is the highest level container for animation data. An animation stack is equivalent to one take of animation.
// An animation stack contains one or more animation layers. If you are doing blended animation, you need more than one animation layer; otherwise, you need only one animation layer.

class FBXAnimationStack
{
public:
    FBXAnimationStack(FBXNode *node);

    void linkLayer(FBXAnimationLayer *layer);

    unsigned long long id = 0;
    unsigned long long localTime = 0;
    unsigned long long referenceTime = 0;

    std::vector<FBXAnimationLayer *> layers;
};
