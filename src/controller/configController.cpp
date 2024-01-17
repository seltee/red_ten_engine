// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "configController.h"

ConfigController::ConfigController()
{
    config = new Config();
}

Config *ConfigController::getConfig()
{
    return config;
}

Config *ConfigController::createConfig()
{
    Config *config = new Config();
    config->copyFrom(this->config);
    return config;
}

void ConfigController::destroyConfig(Config *config)
{
    if (config && config != this->config)
        delete config;
}

void ConfigController::applyNewConfig(Config *config)
{
    if (this->config != config)
        this->config->copyFrom(config);
    viewController->update();
}

void ConfigController::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
}

void ConfigController::setAudioController(AudioController *audioController)
{
    this->audioController = audioController;
}