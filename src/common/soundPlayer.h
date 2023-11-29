// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/childProcess.h"
#include "math/math.h"
#include "controller/audioController.h"
#include "connector/withAudio.h"
#include "resource/resourceSound.h"
#include "audio/audioSource.h"

class SoundPlayer : public ChildProcess, public WithAudio
{
public:
    EXPORT SoundPlayer();
    EXPORT virtual ~SoundPlayer();

    EXPORT void setPosition(Vector3 position);

    EXPORT void setSound(ResourceSound *sound);
    EXPORT void playOnce();
    EXPORT void playLoop();
    EXPORT void stop();
    EXPORT bool isPlaying();
    EXPORT bool isLooping();

    inline void setMaxDistance(float maxDistance) { this->audioSource->setMaxDistance(maxDistance); }
    inline float getMaxDistance() { return this->audioSource->getMaxDistance(); }

    inline void setReferenceDistance(float refDistance) { this->audioSource->setReferenceDistance(refDistance); }
    inline float getReferenceDistance() { return this->audioSource->getReferenceDistance(); }

    inline void setVolume(float volume) { this->audioSource->setVolume(volume); }
    inline float getVolume() { return this->audioSource->getVolume(); }

protected:
    ResourceSound *sound = nullptr;
    AudioSource *audioSource = nullptr;

    bool repeatStream = false;
};