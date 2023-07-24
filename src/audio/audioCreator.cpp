#include "audioCreator.h"
#include "audioBase.h"

AudioBase *AudioCreator::createAudio(Config *config)
{
    return new AudioBase(config);
}