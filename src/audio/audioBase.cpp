#include "audioBase.h"
#include "audio/audioSource.h"
#include <AL/al.h>
#include <AL/alc.h>

AudioBase::AudioBase(Config *config)
{
    std::string prefferedDevice = config->getCurrentAudioDevice();
    ALboolean enumeration = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");

    if (enumeration != AL_FALSE)
    {
        const ALCchar *devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        const ALCchar *device = devices, *next = devices + 1;

        size_t len = 0;

        logger->logf("Audio Devices:");
        while (device && *device != '\0' && next && *next != '\0')
        {
            logger->logff("- %s", device);
            devicesList.push_back(_strdup(device));
            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
        logger->logf("");
    }
    else
    {
        logger->logff("No ALC_ENUMERATION_EXT extension");
        return;
    }
}

void AudioBase::process(float delta)
{
    for (auto it = sources.begin(); it != sources.end(); it++)
    {
        (*it)->process(delta);
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
    ALCdevice *device = alcOpenDevice(deviceName.c_str());
    if (!device)
    {
        logger->logff("Unable to use %s for audio", deviceName.c_str());
        return false;
    }

    ALCcontext *context = alcCreateContext(device, nullptr);
    if (!alcMakeContextCurrent(context))
    {
        logger->logff("Unable to create audio context");
        return false;
    }

    ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    alListener3f(AL_POSITION, 0, 0, 1.0f);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListenerfv(AL_ORIENTATION, listenerOri);
    return true;
}

void AudioBase::setPosition(Vector3 v)
{
    alListener3f(AL_POSITION, v.x, v.y, v.z);
}

void AudioBase::setOrientation(Quat q)
{
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 absoluteDirection = glm::normalize(glm::toMat3(q) * direction);
    ALfloat listenerOri[] = {absoluteDirection.x, absoluteDirection.y, absoluteDirection.z, 0.0f, 1.0f, 0.0f};
    alListenerfv(AL_ORIENTATION, listenerOri);
}

AudioSource *AudioBase::createSource()
{
    auto newSource = new AudioSource();
    sources.push_back(newSource);
    return newSource;
}
