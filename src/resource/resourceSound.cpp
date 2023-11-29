// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "resource/resourceSound.h"
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
};

struct WavChapterHeader
{
    char data[4];
    unsigned int dataSize;
};

ResourceSound::ResourceSound(std::string path) : Resource(path)
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
            logger->logf("%s - wav", path.c_str());
            extension = Extension::WAV;
            bIsStreamable = false;
        }

        if (
            (path[length - 1] == 'g' || path[length - 1] == 'G') &&
            (path[length - 2] == 'g' || path[length - 2] == 'G') &&
            (path[length - 3] == 'o' || path[length - 3] == 'O') &&
            path[length - 4] == '.')
        {
            logger->logf("%s - ogg", path.c_str());
            extension = Extension::OGG;
        }
    }
}

bool ResourceSound::isPath(std::string path)
{
    return this->path == path;
}

bool ResourceSound::isStreamable()
{
    return bIsStreamable;
}

bool ResourceSound::isLoaded()
{
    return bIsLoaded;
}

void ResourceSound::setForceMono(bool state)
{
    bForceMono = state;
}

bool ResourceSound::setupStream(SoundStream *stream)
{
    if (!bIsLoaded)
        return false;

    int error = 0;
    stb_vorbis *vorbis = stb_vorbis_open_filename(path.c_str(), &error, nullptr);

    if (error != 0)
        return false;

    stb_vorbis_info info = stb_vorbis_get_info(vorbis);

    stream->fileReader = vorbis;
    stream->numChannels = info.channels;
    if (info.channels == 1)
        format = AudioFormat::MONO_16;
    if (info.channels == 2)
        format = AudioFormat::STEREO_16;
    this->sampleRate = info.sample_rate;

    return true;
}

int ResourceSound::processStream(SoundStream *stream, unsigned char *buffer, int bufferSize)
{
    if (!bIsLoaded || !isStreamable())
        return 0;

    int shortsRead = stb_vorbis_get_samples_short_interleaved(
        (stb_vorbis *)stream->fileReader,
        stream->numChannels,
        reinterpret_cast<short *>(buffer),
        bufferSize / 2);

    return shortsRead * 2 * stream->numChannels;
}

void ResourceSound::closeStream(SoundStream *stream)
{
    if (!stream)
        return;
    stb_vorbis_close((stb_vorbis *)stream->fileReader);
}

void ResourceSound::load()
{
    if (bIsLoaded)
        return;

    if (extension == Extension::WAV)
        bIsLoaded = loadWAV();

    if (extension == Extension::OGG)
        bIsLoaded = loadOGG();
}

bool ResourceSound::loadWAV()
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
    {
        logger->logff("unable to open file %s", path.c_str());
        return false;
    }

    WavHeader wavHeader;
    fread(&wavHeader, sizeof(WavHeader), 1, file);

    logger->logf("wav loader - %s\nformat %i, num of channels %i, sampleRate %i, byteRate %i, bytesPerSample %i, bitsPerSample %i",
                 path.c_str(), wavHeader.format, wavHeader.numOfChannels, wavHeader.sampleRate, wavHeader.byteRate, wavHeader.bytesPerSample,
                 wavHeader.bitsPerSample);

    int bitsPerSample = wavHeader.bitsPerSample;
    int numOfChannels = wavHeader.numOfChannels;

    // Looking for wav data chapter
    WavChapterHeader wch;
    while (true)
    {
        fread(&wch, sizeof(WavChapterHeader), 1, file);
        if (wch.data[0] == 'd' && wch.data[1] == 'a' && wch.data[2] == 't' && wch.data[3] == 'a')
        {
            dataSize = wch.dataSize;
            break;
        }
        else
        {
            fseek(file, wch.dataSize, SEEK_CUR);
        }

        // Data section wasn't found
        if (feof(file))
        {
            fclose(file);
            return false;
        }
    }

    // Buffer to store the whole file
    data = (unsigned char *)malloc(dataSize);
    if (!data)
    {
        logger->logff("failed to make buffer of %i", dataSize);
        fclose(file);
        return false;
    }
    fread(data, dataSize, 1, file);
    fclose(file);

    bool bIsUsingFloat = wavHeader.format == 3;

    // convert 24 mono to 32 mono
    if (bitsPerSample == 24 && numOfChannels == 1)
    {
        bitsPerSample = 32;
        unsigned char *newData = (unsigned char *)malloc(dataSize + dataSize / 3);
        int *newDataInt = reinterpret_cast<int *>(newData);
        for (int i = 0; i < dataSize / 3; i++)
        {
            newDataInt[i] = ((int)(data[i * 3]) + (int)(data[i * 3 + 1] << 8) + (int)(data[i * 3 + 2] << 16)) << 8;
        }
        free(data);
        data = newData;
        dataSize = dataSize + dataSize / 3;
    }

    // convert 24 stereo to 32 stereo
    if (bitsPerSample == 24 && numOfChannels == 2)
    {
        bitsPerSample = 32;
        unsigned char *newData = (unsigned char *)malloc(dataSize + dataSize / 3);
        int *newDataInt = reinterpret_cast<int *>(newData);
        for (int i = 0; i < dataSize / 6; i++)
        {
            newDataInt[(i << 1)] = ((int)(data[i * 6]) + (int)(data[i * 6 + 1] << 8) + (int)(data[i * 6 + 2] << 16)) << 8;
            newDataInt[(i << 1) + 1] = ((int)(data[i * 6 + 3]) + (int)(data[i * 6 + 4] << 8) + (int)(data[i * 6 + 5] << 16)) << 8;
        }
        free(data);
        data = newData;
        dataSize = dataSize + dataSize / 3;
    }

    if (bForceMono && numOfChannels == 2)
    {
        numOfChannels = 1;
        int newDataSize = dataSize / 2;
        if (bitsPerSample == 8)
        {
            for (int i = 0; i < newDataSize; i++)
                data[i] = data[i << 1];
        }
        if (bitsPerSample == 16)
        {
            short int *dataSInt = reinterpret_cast<short int *>(data);
            for (int i = 0; i < newDataSize / 2; i++)
                dataSInt[i] = dataSInt[i << 1];
        }
        if (bitsPerSample == 32)
        {
            int *dataInt = reinterpret_cast<int *>(data);
            for (int i = 0; i < newDataSize / 4; i++)
                dataInt[i] = dataInt[i << 1];
        }

        dataSize = newDataSize;
    }

    format = AudioFormat::UNKNOWN;

    if (bitsPerSample == 8)
        format = numOfChannels == 2 ? AudioFormat::STEREO_8 : AudioFormat::MONO_8;
    if (bitsPerSample == 16)
        format = numOfChannels == 2 ? AudioFormat::STEREO_16 : AudioFormat::MONO_16;
    if (bitsPerSample == 32 && !bIsUsingFloat)
        format = numOfChannels == 2 ? AudioFormat::STEREO_32 : AudioFormat::MONO_32;
    if (bitsPerSample == 32 && bIsUsingFloat)
        format = numOfChannels == 2 ? AudioFormat::STEREO_32Float : AudioFormat::MONO_32Float;

    if (format == AudioFormat::UNKNOWN)
    {
        free(data);
        logger->logff("Unknown sound format in %s", path.c_str());
        return false;
    }

    sampleRate = wavHeader.sampleRate;
    bIsStreamable = false;
    return true;
}

bool ResourceSound::loadOGG()
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
    {
        logger->logff("unable to open file %s", path.c_str());
        return false;
    }
    fclose(file);
    bIsStreamable = true;
    return true;
}