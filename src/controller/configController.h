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
    EXPORT void applyConfig();
    
    void setViewController(ViewController *viewController);
    void setAudioController(AudioController *audioController);

protected:
    Config *config;
    ViewController *viewController;
    AudioController *audioController;
};