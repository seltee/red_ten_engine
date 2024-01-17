// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/config.h"
#include "common/utils.h"
#include "controller/viewController.h"
#include "controller/audioController.h"

class ConfigController
{
public:
    EXPORT ConfigController();
    EXPORT Config *getConfig();

    EXPORT Config *createConfig();
    EXPORT void destroyConfig(Config *config);

    EXPORT void applyNewConfig(Config *config);
    
    void setViewController(ViewController *viewController);
    void setAudioController(AudioController *audioController);

protected:
    Config *config;
    ViewController *viewController;
    AudioController *audioController;
};