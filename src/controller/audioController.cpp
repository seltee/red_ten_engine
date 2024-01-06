// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "audioController.h"
#include <stdio.h>
#include <string>

AudioController::AudioController(Config *config)
{
    audioBase = new AudioBase(config);
    audioBase->getDevicesList(&devicesList);

    if (devicesList.size() == 0)
    {
        logger->logff("No sound devices found");
        return;
    }

    std::string prefferedDevice = audioBase->getDefaultDeviceName() != "none" ? audioBase->getDefaultDeviceName() : devicesList.at(0);
    config->setCurrentAudioDevice(prefferedDevice);
    logger->logff("Selected %s for audio", prefferedDevice.c_str());

    if (!audioBase->setup(prefferedDevice))
    {
        logger->logff("Unable to use %s for audio", prefferedDevice.c_str());
        return;
    }

    bSoundEnabled = true;
    logger->logff("Sound Enabled using %s", prefferedDevice.c_str());

    audioProcessingTrackerId = profiler->addTracker("Audio processing");
}

AudioController::~AudioController()
{
    delete audioBase;
}

void AudioController::process(float delta)
{
    profiler->startTracking(audioProcessingTrackerId);
    audioBase->process(delta);
    profiler->stopTracking(audioProcessingTrackerId);
}

AudioBase *AudioController::getAudioBase()
{
    return audioBase;
}

void AudioController::setListenerCamera(void *listenerCamera)
{
    this->listenerCamera = listenerCamera;
}

void *AudioController::getListenerCamera()
{
    return listenerCamera;
}
