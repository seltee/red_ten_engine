#include "audioSource.h"
#include <AL/al.h>
#include <AL/alc.h>

std::vector<unsigned int> AudioSource::buffers;

void processStreamChunk(SoundStream *stream, int buffer, int amount)
{
    int format = 0;
    if (stream->format == AudioFormat::MONO_8)
        format = AL_FORMAT_MONO8;
    if (stream->format == AudioFormat::MONO_16)
        format = AL_FORMAT_MONO16;
    if (stream->format == AudioFormat::STEREO_8)
        format = AL_FORMAT_STEREO8;
    if (stream->format == AudioFormat::STEREO_16)
        format = AL_FORMAT_STEREO16;

    alBufferData(
        stream->buffers[buffer],
        format,
        stream->loadBuffer,
        amount,
        stream->sampleRate);
}

AudioSource::AudioSource()
{
    alGenSources((ALuint)1, &sourceID);
    alSourcef(sourceID, AL_PITCH, 1.0f);
    alSourcef(sourceID, AL_GAIN, 1.0f);
    alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(sourceID, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(sourceID, AL_LOOPING, AL_FALSE);

    alSourcei(sourceID, AL_SOURCE_RELATIVE, AL_FALSE);
    alSourcef(sourceID, AL_MAX_DISTANCE, 100.0f);
    alSourcef(sourceID, AL_REFERENCE_DISTANCE, 1.0f);
}

AudioSource::~AudioSource()
{
    alDeleteSources(1, &sourceID);
}

void AudioSource::process(float delta)
{
    if (state == AudioSourceState::Playing)
    {
        int internalState;
        alGetSourcei(sourceID, AL_SOURCE_STATE, &internalState);
        if (internalState != AL_PLAYING)
        {
            state = AudioSourceState::Stopped;
        }
    }

    if (sound && sound->isStreamable() && state == AudioSourceState::Playing)
    {
        ALint buffersProcessed = 0;
        alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, &buffersProcessed);

        while (buffersProcessed > 0)
        {
            alSourceUnqueueBuffers(sourceID, 1, &soundStream->buffers[soundStream->lastLoaded % SOUND_BUFFERS_AMOUNT]);
            bool res = sound->processBuffers(soundStream, processStreamChunk);
            if (res)
            {
                alSourceQueueBuffers(sourceID, 1, &soundStream->buffers[(soundStream->lastLoaded - 1) % SOUND_BUFFERS_AMOUNT]);
            }
            buffersProcessed--;
        }
    }

    if (sound && sound->isStreamable() && state == AudioSourceState::Stopped && soundStream)
    {
        closeStream();
        if (repeatStream)
            loop(this->sound);
    }
}

void AudioSource::setPitch(float pitch)
{
    alSourcef(sourceID, AL_PITCH, pitch);
}

void AudioSource::setGain(float gain)
{
    alSourcef(sourceID, AL_GAIN, gain);
}

void AudioSource::setPosition(Vector3 p)
{
    alSource3f(sourceID, AL_POSITION, -p.x, p.y, p.z);
}

void AudioSource::setVelocity(Vector3 v)
{
    alSource3f(sourceID, AL_VELOCITY, v.x, v.y, v.z);
}

void AudioSource::setMaxDistance(float maxDistance)
{
    alSourcef(sourceID, AL_MAX_DISTANCE, maxDistance);
}

float AudioSource::getMaxDistance()
{
    float out;
    alGetSourcef(sourceID, AL_MAX_DISTANCE, &out);
    return out;
}

void AudioSource::setReferenceDistance(float referenceDistance)
{
    alSourcef(sourceID, AL_REFERENCE_DISTANCE, referenceDistance);
}

float AudioSource::getReferenceDistance()
{
    float out;
    alGetSourcef(sourceID, AL_REFERENCE_DISTANCE, &out);
    return out;
}

void AudioSource::play(Sound *sound)
{
    processNewSound(sound);

    if (sound->isLoaded() && !sound->isStreamable())
    {
        alSourcei(sourceID, AL_BUFFER, buffers.at(sound->getId()));
        alSourcei(sourceID, AL_LOOPING, AL_FALSE);
        alSourcePlay(sourceID);
        state = AudioSourceState::Playing;
    }
    if (sound->isLoaded() && sound->isStreamable())
    {

        /*
        if (soundStream)
            sound->closeBuffer(soundStream);

        soundStream = sound->createNewStream();
        if (soundStream)
        {
            sound->processBuffers(soundStream);
            alSourceQueueBuffers(sourceID, SOUND_BUFFERS_AMOUNT, soundStream->buffers);
            alSourcePlay(sourceID);
            state = AL_PLAYING;
            repeatStream = false;
        }
        */
    }
}

void AudioSource::loop(Sound *sound)
{
    processNewSound(sound);

    if (sound->isLoaded() && !sound->isStreamable())
    {
        alSourcei(sourceID, AL_BUFFER, buffers.at(sound->getId()));
        alSourcei(sourceID, AL_LOOPING, AL_TRUE);
        alSourcePlay(sourceID);
        state = AudioSourceState::Playing;
    }
    if (sound->isLoaded() && sound->isStreamable())
    {
        if (soundStream)
            closeStream();
        soundStream = new SoundStream();
        alGenBuffers((ALuint)SOUND_BUFFERS_AMOUNT, soundStream->buffers);

        if (sound->setupStream(soundStream))
        {
            state = AudioSourceState::Playing;
            repeatStream = true;

            sound->processBuffers(soundStream, processStreamChunk);
            alSourceQueueBuffers(sourceID, SOUND_BUFFERS_AMOUNT, soundStream->buffers);
            alSourcePlay(sourceID);
        }
    }
}

void AudioSource::stop()
{
    alSourceStop(sourceID);
}

void AudioSource::processNewSound(Sound *sound)
{
    if (sound->getId() == -1 && !sound->isStreamable())
    {
        sound->load();
        if (sound->isLoaded())
        {
            int newId = buffers.size();
            sound->setId(newId);
            unsigned int newBufferId;
            alGenBuffers((ALuint)1, &newBufferId);
            buffers.push_back(newBufferId);

            int format = 0;
            if (sound->getAudioFormat() == AudioFormat::MONO_8)
                format = AL_FORMAT_MONO8;
            if (sound->getAudioFormat() == AudioFormat::MONO_16)
                format = AL_FORMAT_MONO16;
            if (sound->getAudioFormat() == AudioFormat::STEREO_8)
                format = AL_FORMAT_STEREO8;
            if (sound->getAudioFormat() == AudioFormat::STEREO_16)
                format = AL_FORMAT_STEREO16;

            alBufferData(
                newBufferId,
                format,
                sound->getData(),
                sound->getDataSize(),
                sound->getSampleRate());
        }
    }
    if (sound->isStreamable())
    {
        sound->load();
    }
    this->sound = sound;
}

void AudioSource::closeStream()
{
    if (soundStream)
    {
        sound->closeBuffer(soundStream);
        alDeleteBuffers(SOUND_BUFFERS_AMOUNT, soundStream->buffers);
        delete soundStream;
        soundStream = nullptr;
    }
}