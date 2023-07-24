#pragma once
#include "math/math.h"
#include "common/destroyable.h"
#include "resource/sound.h"

class AudioBase;

enum class AudioSourceState
{
    Stopped = 0,
    Playing
};

class AudioSource : public Destroyable
{
public:
    AudioSource();
    ~AudioSource();

    void process(float delta);

    void setPitch(float pitch);
    void setGain(float gain);
    void setPosition(Vector3 p);
    void setVelocity(Vector3 v);

    void setMaxDistance(float maxDistance);
    float getMaxDistance();
    void setReferenceDistance(float referenceDistance);
    float getReferenceDistance();

    inline AudioSourceState getState() { return state; }
    inline Sound *getSound() { return this->sound; }

    void play(Sound *sound);
    void loop(Sound *sound);
    void stop();

protected:
    void processNewSound(Sound *sound);
    void closeStream();

    bool bIsLooping = false;

    AudioSourceState state = AudioSourceState::Stopped;
    Sound *sound = nullptr;

    unsigned int sourceID;
    static std::vector<unsigned int> buffers;

    SoundStream *soundStream = nullptr;
    bool repeatStream = false;
};
