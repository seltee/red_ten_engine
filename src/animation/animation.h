// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "animation/animationTarget.h"
#include <string>
#include <vector>

class Animation
{
public:
    Animation(std::string name);

    AnimationTarget *getAnimationTarget(std::string targetName);
    AnimationTarget *createAnimationTarget(std::string targetName);

    bool getAnimationTransformation(std::string &name, float timeStamp, Transformation *transformation);

    void recalcAnimationLength();

    inline float getTimeLength() { return timeLength; }

    inline std::string getName() { return name; }

protected:
    std::string name;
    std::vector<AnimationTarget *> targets;
    float timeLength = 0.0f;
};