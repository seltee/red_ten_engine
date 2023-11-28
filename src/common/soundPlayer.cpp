// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "soundPlayer.h"
#include "math/math.h"

SoundPlayer::SoundPlayer()
{
    audioSource = audio->getAudioBase()->createSource();
    audioSource->setMaxDistance(4000.0f);
    audioSource->setReferenceDistance(2000.0f);
}

SoundPlayer::~SoundPlayer()
{
    audioSource->destroy();
}

void SoundPlayer::setPosition(Vector3 position)
{
    audioSource->setPosition(position);
}

void SoundPlayer::setSound(Sound *sound)
{
    stop();
    this->sound = sound;
}

void SoundPlayer::playOnce()
{
    if (sound && audioSource)
    {
        audioSource->play(sound);
    }
}

void SoundPlayer::playLoop()
{
    if (sound && audioSource)
    {
        audioSource->loop(sound);
    }
}

void SoundPlayer::stop()
{
    if (audioSource)
    {
        audioSource->stop();
    }
}

bool SoundPlayer::isPlaying()
{
    if (sound)
    {
        return audioSource->getState() == AudioSourceState::Playing;
    }
    return false;
}

bool SoundPlayer::isLooping()
{
    if (sound)
    {
        return audioSource->getState() == AudioSourceState::Playing && audioSource->isLooping();
    }
    return false;
}
