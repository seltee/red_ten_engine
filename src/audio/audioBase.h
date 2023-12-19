// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "math/math.h"
#include "common/config.h"
#include "connector/withLogger.h"
#include <string>
#include <vector>

class AudioSource;

class AudioBase : public WithLogger
{
public:
    AudioBase(Config *config);
    ~AudioBase();
    void process(float delta);

    void getDevicesList(std::vector<std::string> *list);
    bool setup(std::string deviceName);

    void setPosition(Vector3 vPosition);
    inline Vector3 getPosition() { return vPosition; }

    void setOrientation(Quat qOrientation);
    inline Vector3 getDirection() { return vDirection; }

    AudioSource *createSource();

    inline int getFrequency() { return freq; }
    inline int getChannelsCount() { return channels; }
    inline int getFormat() { return format; }
    inline std::vector<AudioSource *> *getSources() { return &sources; }
    inline unsigned char *getBuffer() { return buffer; }

    inline std::string getDefaultDeviceName() { return defaultAudioName; };

protected:
    std::vector<AudioSource *> sources;
    std::vector<std::string> devicesList;

    int deviceId = 0;

    int freq = 0;
    int channels = 0;
    int format = 0;
    int bufferSize = 0;

    unsigned char *buffer = nullptr;

    Vector3 vPosition;
    Vector3 vDirection;

    std::string defaultAudioName;
};