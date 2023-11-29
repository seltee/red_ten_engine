// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT


#pragma once
#include "math/math.h"
#include "common/destroyable.h"
#include "resource/resourceSound.h"

class AudioBase;

enum class AudioSourceState
{
    Stopped = 0,
    Playing
};

class AudioSource : public Destroyable, WithLogger
{
public:
    AudioSource(int format, int freq, int channels);
    virtual ~AudioSource();

    void fillBuffer(unsigned char *buffer, int lengthInSamples);

    void process(float delta);

    inline void setPosition(Vector3 vPosition) { this->vPosition = vPosition; }
    inline Vector3 getPosition() { return this->vPosition; }

    inline void setVolume(float volume) { this->volume = volume; }
    inline float getVolume() { return this->volume; }

    inline void setMaxDistance(float maxDistance) { this->maxDistance = maxDistance; }
    inline float getMaxDistance() { return this->maxDistance; }
    inline void setReferenceDistance(float referenceDistance) { this->refDistance = referenceDistance; }
    inline float getReferenceDistance() { return this->refDistance; }

    inline void setAllow3dPositioning(bool state) { this->bAllow3dPositioning = state; }
    inline bool is3dPositioningAllowed() { return this->bAllow3dPositioning; }

    inline bool isLooping() { return this->bIsLooping; }

    inline AudioSourceState getState() { return state; }
    inline ResourceSound *getSound() { return this->sound; }

    void play(ResourceSound *sound);
    void loop(ResourceSound *sound);
    void stop();

    // bAllow3dPositioning is true and sound format is mono
    bool is3dPositionable();

protected:
    void setupBuffers(int bufferSize);

    bool bIsLooping = false;

    AudioSourceState state = AudioSourceState::Stopped;
    ResourceSound *sound = nullptr;

    unsigned int sourceID;
    static std::vector<unsigned int> buffers;

    SoundStream *soundStream = nullptr;
    float playPosition = 0.0f;
    float playStep = 0.0f;

    int format = 0;
    int freq = 0;
    int channels = 0;

    float volume = 1.0f;

    float maxDistance = 100.0f;
    float refDistance = 10.0f;

    bool bAllow3dPositioning = true;

    Vector3 vPosition = Vector3(0.0f, 0.0f, 0.0f);

    unsigned char *streamBuffers[2] = {nullptr, nullptr};
    int bufferAudioBytes[2] = {0, 0};

    int bufferSize = 0;
    int currentBuffer = 0;
    int lastLoadedBuffer = 0;
};
