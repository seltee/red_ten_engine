// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"

class AudioController;

class WithAudio
{
public:
    EXPORT WithAudio();
    static void setAudioController(AudioController *audioController);

protected:
    AudioController *audio;

protected:
    static AudioController *audioController;
};
