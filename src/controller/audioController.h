// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "common/childProcess.h"
#include "common/config.h"
#include "connector/withLogger.h"
#include "audio/audioBase.h"
#include "resource/resourceSound.h"
#include "connector/withProfiler.h"
#include <vector>
#include <string>

class AudioController : public WithLogger, public WithProfiler
{
public:
    AudioController(Config *config);
    ~AudioController();

    void update();

    void process(float delta);

    AudioBase *getAudioBase();

    void setListenerCamera(void *listenerCamera);
    void *getListenerCamera();

protected:
    Config *config;
    bool bSoundEnabled = false;
    std::vector<std::string> devicesList;
    std::vector<ChildProcess *> soundPlayers;

    void *listenerCamera = nullptr;

    AudioBase *audioBase = nullptr;

    int audioProcessingTrackerId = 0;
};
