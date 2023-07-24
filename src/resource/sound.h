// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

// supports formats:
// usual: .wav,
// streaming: .ogg

#pragma once
#include "common/utils.h"
#include "connector/withLogger.h"
#include "audio/audioBase.h"
#include <string>

#define SOUND_BUFFER_SIZE (1024 * 128)
#define SOUND_BUFFERS_AMOUNT 4

enum class AudioFormat
{
    UNKNOWN = 0,
    MONO_8,
    MONO_16,
    STEREO_8,
    STEREO_16
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
    short *loadBuffer;
    unsigned int buffers[SOUND_BUFFERS_AMOUNT];
    int numChannels;
    int sampleRate;
    AudioFormat format;
    int lastLoaded;
};

class Sound : public WithLogger
{
public:
    EXPORT Sound(std::string path);

    EXPORT bool isPath(std::string path);
    EXPORT bool isStreamable();
    EXPORT bool isLoaded();
    EXPORT void setForceMono(bool state);
    EXPORT bool setupStream(SoundStream *stream);
    EXPORT bool processBuffers(SoundStream *stream, void (*processBuffer) (SoundStream *stream, int buffer, int amount));
    EXPORT void closeBuffer(SoundStream *stream);
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
