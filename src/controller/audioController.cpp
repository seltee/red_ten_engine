// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "audioController.h"
#include "audio/audioCreator.h"
#include <stdio.h>
#include <string>

AudioController::AudioController(Config *config)
{
    audioBase = AudioCreator::createAudio(config);
    audioBase->getDevicesList(&devicesList);
    std::string prefferedDevice;

    if (devicesList.size() == 0)
    {
        logger->logff("No sound devices found");
        logger->logff("Installation of OpenAL libraries might be required");
        return;
    }

    prefferedDevice = devicesList.at(0);
    config->setCurrentAudioDevice(prefferedDevice);
    logger->logff("Auto selected %s for audio", prefferedDevice.c_str());

    if (!audioBase->setup(prefferedDevice))
    {
        logger->logff("Unable to use %s for audio", prefferedDevice.c_str());
        return;
    }

    bSoundEnabled = true;
    logger->logff("Sound Enabled using %s", prefferedDevice.c_str());

    audioProcessingTrackerId = profiler->addTracker("Audio processing");
}

void AudioController::update()
{
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
