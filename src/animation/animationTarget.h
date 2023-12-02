// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "math/transformation.h"
#include "common/utils.h"
#include <string>

// Time in timestamp is 1.0 = 1000 ms or 1 second
// Due to float precision sweat spot

struct AnimationKeyTranform
{
    float timeStamp;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

class AnimationTarget
{
public:
    EXPORT AnimationTarget(std::string targetName);

    EXPORT float getAnimationTimeLength();

    EXPORT void getTransformByTime(float timeStamp, Transformation *transformation);

    inline bool isName(std::string targetName) { return this->targetName == targetName; }
    inline std::string getTargetName() { return targetName; }

    void addKey(AnimationKeyTranform keyTransform);

protected:
    std::string targetName;
    std::vector<AnimationKeyTranform> keys;
};