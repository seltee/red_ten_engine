// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/destroyable.h"
#include "animation/animation.h"

class Animator : public Destroyable
{
public:
    Animator(Animation *animation);

    void process(float delta);

    float time = 0.0f;
    float blend = 1.0f;
    float speed = 1.0f;

    inline void play() { bIsPlaying = true; }
    inline void play(float from)
    {
        bIsPlaying = true;
        time = from;
    }
    inline void stop() { bIsPlaying = false; }
    inline void rewind() { time = 0.0f; }
    inline void setBlend(float blend) { this->blend = blend; }
    inline bool isPlaying() { return bIsPlaying; }

    inline Animation *getAnimation() { return animation; }

protected:
    Animation *animation = nullptr;
    bool bIsPlaying = false;
};
