// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "audioBase.h"
#include "audio/audioSource.h"
#include <SDL.h>
#include "rtengine.h"

void audioCallback(void *userdata, unsigned char *stream, int len);

const Vector3 vUpDirection = Vector3(0.0f, 1.0f, 0.0f);

AudioBase::AudioBase(Config *config)
{
    std::string prefferedDevice = config->getCurrentAudioDevice();

    const int audioDevices = SDL_GetNumAudioDevices(0);
    if (audioDevices > 0)
    {
        logger->logf("Audio Devices:");
        for (int i = 0; i < audioDevices; i++)
        {
            const char *deviceName = SDL_GetAudioDeviceName(i, 0);
            logger->logff("- %s", deviceName);
            devicesList.push_back(_strdup(deviceName));
        }
        logger->logf("");
    }

    SDL_AudioSpec spec;
    char *defaultAudioName = nullptr;
    SDL_GetDefaultAudioInfo(&defaultAudioName, &spec, 0);

    if (defaultAudioName)
    {
        logger->logf("Default auido device: %s, freq %i\n", defaultAudioName, spec.freq);
        this->defaultAudioName = defaultAudioName;
        SDL_free(defaultAudioName);
    }
    else
    {
        this->defaultAudioName = "none";
        logger->logf("No default audio device\n");
    }
}

AudioBase::~AudioBase()
{
    if (deviceId > 0)
        SDL_CloseAudioDevice(deviceId);

    if (buffer)
        delete buffer;
}

void AudioBase::process(float delta)
{
    auto it = sources.begin();
    while (it != sources.end())
    {
        (*it)->process(delta);
        if ((*it)->isDestroyed())
        {
            delete (*it);
            it = sources.erase(it);
        }
        else
            ++it;
    }
}

void AudioBase::getDevicesList(std::vector<std::string> *list)
{
    for (auto it = devicesList.begin(); it != devicesList.end(); it++)
    {
        list->push_back(std::string(it->c_str()));
    }
}

bool AudioBase::setup(std::string deviceName)
{
    SDL_AudioSpec want, have;
    memset(&want, 0, sizeof(want));
    want.freq = 96000;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 0;
    want.callback = audioCallback;
    want.userdata = this;

    deviceId = SDL_OpenAudioDevice(deviceName.c_str(), 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
    if (deviceId > 0)
    {
        freq = have.freq;
        channels = have.channels;
        format = have.format;
        bufferSize = have.size;
        buffer = new unsigned char[bufferSize];

        logger->logff("Audio info: %s, channels: %i, frequency: %i, format: %i, buffer size: %i", deviceName.c_str(), channels, freq, format, bufferSize);
        SDL_PauseAudioDevice(deviceId, 0);

        return true;
    }

    logger->logff("Unable to use %s for audio, error %i", deviceName.c_str(), deviceId);

    return false;
}

void AudioBase::setPosition(Vector3 vPosition)
{
    this->vPosition = vPosition;
}

void AudioBase::setDirection(Vector3 vDirection)
{
    this->vDirection = glm::length2(vDirection) != 0.0f ? vDirection : Vector3(0.0f, 0.0f, 1.0f);
}

AudioSource *AudioBase::createSource()
{
    auto newSource = new AudioSource(format, freq, channels);
    sources.push_back(newSource);
    return newSource;
}

void audioCallback(void *userdata, unsigned char *stream, int len)
{
    AudioBase *base = reinterpret_cast<AudioBase *>(userdata);

    memset(stream, 0, len);

    if (base->getFormat() == AUDIO_F32)
    {
        float *fsnd = reinterpret_cast<float *>(stream);

        len /= sizeof(float);
        int amountOfSamples = len / base->getChannelsCount();
        auto sources = base->getSources();
        auto fillBuffer = base->getBuffer();
        float *nativeFillBuffer = reinterpret_cast<float *>(base->getBuffer());

        if (!fillBuffer || !nativeFillBuffer || sources->size() == 0)
            return;

        Vector3 basePosition = base->getPosition();
        Vector3 baseDirection = base->getDirection();

        for (auto &it : *sources)
        {
            if (it->getState() == AudioSourceState::Playing)
            {
                if (it->is3dPositionable())
                {
                    // Check if source is in sound range
                    Vector3 localPosition = it->getPosition() - basePosition;
                    float maxDistance = it->getMaxDistance();
                    float distance = glm::length(localPosition);

                    if (distance > maxDistance)
                        continue;

                    // Calc left/right relations
                    Vector3 baseSide = glm::cross(baseDirection, vUpDirection);

                    float sideProjection = glm::dot(baseSide, glm::normalize(localPosition));
                    float mainValue = 1.0f - fabsf(sideProjection);
                    float lVolume, rVolume;
                    if (sideProjection < 0.0f)
                    {
                        lVolume = mainValue;
                        rVolume = mainValue + fabsf(sideProjection);
                    }
                    else
                    {
                        lVolume = mainValue + fabsf(sideProjection);
                        rVolume = mainValue;
                    }

                    // Volume based on distance
                    float distanceVolume = 1.0f;
                    float refDistance = it->getReferenceDistance();
                    if (distance > refDistance)
                    {
                        distanceVolume = 1.0f - (distance - refDistance) / (maxDistance - refDistance);
                    }
                    lVolume *= distanceVolume;
                    rVolume *= distanceVolume;

                    it->fillBuffer(fillBuffer, amountOfSamples);
                    for (int i = 0; i < len; i += 2)
                    {
                        // L
                        fsnd[i] += nativeFillBuffer[i] * lVolume;
                        // R
                        fsnd[i + 1] += nativeFillBuffer[i + 1] * rVolume;
                    }
                }
                else
                {
                    it->fillBuffer(fillBuffer, amountOfSamples);
                    for (int i = 0; i < len; i++)
                    {
                        fsnd[i] += nativeFillBuffer[i];
                    }
                }
            }
            for (int i = 0; i < len; i++)
            {
                fsnd[i] = fminf(fmaxf(fsnd[i], -1.0f), 1.0f);
            }
        }

        return;
    }
}