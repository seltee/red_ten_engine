// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "soundController.h"
#include "audio/audioCreator.h"
#include <stdio.h>
#include <string>

SoundController::SoundController(Config *config)
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
}

void SoundController::update()
{
}

void SoundController::process(float delta)
{
    audioBase->process(delta);
}

AudioBase *SoundController::getAudioBase()
{
    return audioBase;
}

void SoundController::setListenerCamera(void *listenerCamera)
{
    this->listenerCamera = listenerCamera;
}

void *SoundController::getListenerCamera()
{
    return listenerCamera;
}
