#pragma once
#include <string>
#include <vector>
#include "math/math.h"
#include "common/config.h"
#include "connector/withLogger.h"

class AudioSource;

class AudioBase : public WithLogger
{
public:
    AudioBase(Config *config);
    void process(float delta);
    
    void getDevicesList(std::vector<std::string> *list);
    bool setup(std::string deviceName);

    void setPosition(Vector3 v);
    void setOrientation(Quat q);

    AudioSource *createSource();

protected:
    std::vector<AudioSource*> sources;
    std::vector<std::string> devicesList;
};