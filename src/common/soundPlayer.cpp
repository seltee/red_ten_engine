// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "soundPlayer.h"
#include "math/math.h"

AudioController *SoundPlayer::audioController = nullptr;

SoundPlayer::SoundPlayer()
{
    audioSource = audioController->getAudioBase()->createSource();
    audioSource->setMaxDistance(4000.0f);
    audioSource->setReferenceDistance(2000.0f);
}

SoundPlayer::~SoundPlayer()
{
    if (soundStream)
        sound->closeBuffer(soundStream);

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
    /*
    if (sound)
    {
        if (sound->getBuffer() && !sound->isStreamable())
        {
            // int isLooping;
            // alGetSourcei(sourceID, AL_LOOPING, &isLooping);
            // alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
            // return (isLooping != AL_FALSE) && (state == AL_PLAYING);
        }
        if (sound->isStreamable() && repeatStream && isPlaying())
        {
            return true;
        }
    }
    */
    return false;
}

void SoundPlayer::setMaxDistance(float maxDistance)
{
    audioSource->setMaxDistance(maxDistance);
}

float SoundPlayer::getMaxDistance()
{
    return audioSource->getMaxDistance();
}

void SoundPlayer::setRefereneceDistance(float referenceDistance)
{
    audioSource->setReferenceDistance(referenceDistance);
}

float SoundPlayer::getReferenceDistance()
{
    return audioSource->getReferenceDistance();
}

void SoundPlayer::setVolume(float volume)
{
}

float SoundPlayer::getVolume()
{
    return 1.0f;
}

void SoundPlayer::setAudioController(AudioController *audioController)
{
    SoundPlayer::audioController = audioController;
}