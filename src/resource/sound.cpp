// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/sound.h"
#include "loaders/stb_vorbis.h"
#include <stdio.h>
#include <string>

struct WavHeader
{
    char mark[4];
    int fileSize;
    char wave[4];
    char fmt[4];
    int formatLength;
    unsigned short int format;
    unsigned short int numOfChannels;
    int sampleRate;
    int byteRate;
    unsigned short int bytesPerSample;
    unsigned short int bitsPerSample;
    char data[4];
    unsigned int dataSize;
};

Sound::Sound(std::string path)
{
    this->path = path;
    int length = path.length();
    if (length > 5)
    {
        if (
            (path[length - 1] == 'v' || path[length - 1] == 'V') &&
            (path[length - 2] == 'a' || path[length - 2] == 'A') &&
            (path[length - 3] == 'w' || path[length - 3] == 'W') &&
            path[length - 4] == '.')
        {
            logger->logf("%s - wav\n", path.c_str());
            extension = Extension::WAV;
            bIsStreamable = false;
        }

        if (
            (path[length - 1] == 'g' || path[length - 1] == 'G') &&
            (path[length - 2] == 'g' || path[length - 2] == 'G') &&
            (path[length - 3] == 'o' || path[length - 3] == 'O') &&
            path[length - 4] == '.')
        {
            logger->logf("%s - ogg\n", path.c_str());
            extension = Extension::OGG;
        }
    }
}

bool Sound::isPath(std::string path)
{
    return this->path == path;
}

bool Sound::isStreamable()
{
    return bIsStreamable;
}

bool Sound::isLoaded()
{
    return bIsLoaded;
}

void Sound::setForceMono(bool state)
{
    bForceMono = state;
}

bool Sound::setupStream(SoundStream *stream)
{
    if (!bIsLoaded)
        return false;

    int error = 0;
    stb_vorbis *vorbis = stb_vorbis_open_filename(path.c_str(), &error, nullptr);

    if (error != 0)
        return false;

    stb_vorbis_info info = stb_vorbis_get_info(vorbis);

    stream->fileReader = vorbis;
    stream->loadBuffer = (short *)malloc(sizeof(short) * SOUND_BUFFER_SIZE * info.channels);

    stream->numChannels = info.channels;
    stream->sampleRate = info.sample_rate;
    if (info.channels == 1)
        stream->format = AudioFormat::MONO_16;
    if (info.channels == 2)
        stream->format = AudioFormat::STEREO_16;
    stream->lastLoaded = 0;

    return true;
}

bool Sound::processBuffers(SoundStream *stream, void (*processBuffer) (SoundStream *stream, int buffer, int amount))
{
    if (!bIsLoaded || !isStreamable())
        return false;

    int amount = stream->lastLoaded == 0 ? SOUND_BUFFERS_AMOUNT : 1;

    for (int i = 0; i < amount; i++)
    {
        int loaded = stb_vorbis_get_samples_short_interleaved((stb_vorbis *)stream->fileReader, stream->numChannels, stream->loadBuffer, SOUND_BUFFER_SIZE);
        if (loaded)
        {
            processBuffer(stream, stream->lastLoaded % SOUND_BUFFERS_AMOUNT, loaded * stream->numChannels * 2);
        }
        else
            return false;
        stream->lastLoaded++;
    }

    return true;
}

void Sound::closeBuffer(SoundStream *stream)
{
    if (!stream)
        return;
    stb_vorbis_close((stb_vorbis *)stream->fileReader);
}

void Sound::load()
{
    if (bIsLoaded)
        return;

    if (extension == Extension::WAV)
        bIsLoaded = loadWAV();

    if (extension == Extension::OGG)
        bIsLoaded = loadOGG();
}

bool Sound::loadWAV()
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
    {
        logger->logff("unable to open file %s\n", path.c_str());
        return false;
    }

    WavHeader wavHeader;
    fread(&wavHeader, sizeof(WavHeader), 1, file);

    logger->logf("wav loader - %s\nformat %i, num of channels %i, sampleRate %i, byteRate %i, bytesPerSample %i, bitsPerSample %i, data size %i\n",
                 path.c_str(), wavHeader.format, wavHeader.numOfChannels, wavHeader.sampleRate, wavHeader.byteRate, wavHeader.bytesPerSample,
                 wavHeader.bitsPerSample, wavHeader.dataSize);

    dataSize = wavHeader.dataSize;
    data = (unsigned char *)malloc(dataSize);
    if (!data)
    {
        logger->logff("failed to make buffer of %i\n", dataSize);
        fclose(file);
        return false;
    }
    fread(data, dataSize, 1, file);
    fclose(file);

    if (bForceMono && wavHeader.numOfChannels == 2)
    {
        wavHeader.numOfChannels = 1;
        int newDataSize = dataSize / 2;
        if (wavHeader.bitsPerSample == 8)
            for (int i = 0; i < newDataSize; i++)
                data[i] = data[i << 1];
        else
            for (int i = 0; i < newDataSize / 2; i++)
                *(short int *)(&data[i << 1]) = *(short int *)(&data[(i << 2) + 2]);

        dataSize = newDataSize;
    }

    format = AudioFormat::UNKNOWN;
    if (wavHeader.numOfChannels == 1)
    {
        if (wavHeader.bitsPerSample == 8)
            format = AudioFormat::MONO_8;
        if (wavHeader.bitsPerSample == 16)
            format = AudioFormat::MONO_16;
    }
    if (wavHeader.numOfChannels == 2)
    {
        if (wavHeader.bitsPerSample == 8)
            format = AudioFormat::STEREO_8;
        if (wavHeader.bitsPerSample == 16)
            format = AudioFormat::STEREO_16;
    }

    if (format == AudioFormat::UNKNOWN)
    {
        free(data);
        logger->logff("Unknown sound format in %s\n", path.c_str());
        return false;
    }

    sampleRate = wavHeader.sampleRate;
    bIsStreamable = false;
    return true;
}

bool Sound::loadOGG()
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
    {
        logger->logff("unable to open file %s\n", path.c_str());
        return false;
    }
    fclose(file);
    bIsStreamable = true;
    return true;
}