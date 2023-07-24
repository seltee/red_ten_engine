#pragma once
#include "common/config.h"

class AudioBase;

class AudioCreator
{
public:
    static AudioBase *createAudio(Config *config);
};