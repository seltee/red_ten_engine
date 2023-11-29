// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

// supports formats:
// usual: .wav,
// streaming: .ogg

#pragma once
#include "resource/resource.h"
#include "common/utils.h"
#include "connector/withLogger.h"
#include "audio/audioBase.h"
#include <string>

enum class AudioFormat
{
    UNKNOWN = 0,
    MONO_8,
    MONO_16,
    MONO_32,
    MONO_32Float,
    STEREO_8,
    STEREO_16,
    STEREO_32,
    STEREO_32Float
};

enum Extension
{
    UNKNOWN = 0,
    WAV,
    OGG
};

struct SoundStream
{
    void *fileReader;
    int numChannels;
};

class ResourceSound : public WithLogger, public Resource
{
public:
    EXPORT ResourceSound(std::string path);

    EXPORT bool isPath(std::string path);
    EXPORT bool isStreamable();
    EXPORT bool isLoaded();
    EXPORT void setForceMono(bool state);
    EXPORT bool setupStream(SoundStream *stream);
    EXPORT int processStream(SoundStream *stream, unsigned char *buffer, int bufferSize);
    EXPORT void closeStream(SoundStream *stream);
    EXPORT void load();

    inline void setId(int id) { this->id = id; }
    inline int getId() { return id; }

    inline unsigned char *getData()
    {
        load();
        return data;
    }

    inline int getDataSize()
    {
        load();
        return dataSize;
    }

    inline AudioFormat getAudioFormat()
    {
        load();
        return format;
    }

    inline unsigned int getSampleRate()
    {
        load();
        return sampleRate;
    }

protected:
    bool loadWAV();
    bool loadOGG();

    std::string path;
    bool bIsStreamable;
    bool bIsLoaded = false;
    bool bForceMono = false;
    Extension extension = Extension::UNKNOWN;

    int id = -1;
    unsigned char *data;
    unsigned int dataSize;
    AudioFormat format;
    unsigned int sampleRate;
};
