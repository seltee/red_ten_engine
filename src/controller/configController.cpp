#include "configController.h"

ConfigController::ConfigController()
{
    config = new Config();
}

Config *ConfigController::getConfig()
{
    return config;
}

void ConfigController::applyConfig()
{
    viewController->update();
    audioController->update();
}

void ConfigController::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
}

void ConfigController::setAudioController(AudioController *audioController) {
    this->audioController = audioController;
}