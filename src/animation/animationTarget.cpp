// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "animation/animationTarget.h"

AnimationTarget::AnimationTarget(std::string targetName)
{
    this->targetName = targetName;
}

float AnimationTarget::getAnimationTimeLength()
{
    if (keys.size() > 0)
    {
        return keys.at(keys.size() - 1).timeStamp;
    }
    return 0.0f;
}

void AnimationTarget::addKey(AnimationKeyTranform keyTransform)
{
    keys.push_back(keyTransform);
}

void AnimationTarget::getTransformByTime(float timeStamp, Transformation *transformation)
{
    int size = keys.size();
    for (int i = 0; i < size; i++)
    {
        if (timeStamp < keys[i].timeStamp)
        {
            if (i == 0)
            {
                transformation->setPosition(keys[i].position);
                transformation->setRotation(keys[i].rotation);
                transformation->setScale(keys[i].scale);
            }
            else
            {
                float time = (timeStamp - keys[i - 1].timeStamp) / (keys[i].timeStamp - keys[i - 1].timeStamp);
                transformation->setPosition(keys[i - 1].position * (1.0f - time) + keys[i].position * time);
                transformation->setRotation(keys[i - 1].rotation * (1.0f - time) + keys[i].rotation * time);
                transformation->setScale(keys[i - 1].scale * (1.0f - time) + keys[i].scale * time);
            }
            break;
        }
    }
}