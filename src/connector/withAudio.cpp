// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "withAudio.h"
#include "controller/audioController.h"

AudioController *WithAudio::audioController = nullptr;

WithAudio::WithAudio()
{
    this->audio = WithAudio::audioController;
}

void WithAudio::setAudioController(AudioController *audioController)
{
    WithAudio::audioController = audioController;
}
