// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "resource/resourceSound.h"

// returns bool isFinished
inline bool fillFloatStereoBufferFromMono8(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    unsigned char *soundData = reinterpret_cast<unsigned char *>(data);

    const int sampleSize = 1;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position);
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float out = ((float)((char)(soundData[samplePosition] - 128))) / (float)((unsigned int)0x80) * volume;
        fbuffer[(sample << 1)] = out;
        fbuffer[(sample << 1) + 1] = out;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromMono16(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    short *soundData = reinterpret_cast<short *>(data);

    const int sampleSize = 2;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position);
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float out = ((float)soundData[samplePosition]) / (float)((unsigned int)0x8000) * volume;
        fbuffer[(sample << 1)] = out;
        fbuffer[(sample << 1) + 1] = out;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromMono32(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    int *soundData = reinterpret_cast<int *>(data);

    const int sampleSize = 4;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position);
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float out = ((float)soundData[samplePosition]) / (float)((unsigned int)0x80000000) * volume;
        fbuffer[(sample << 1)] = out;
        fbuffer[(sample << 1) + 1] = out;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromMono32Float(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    float *soundData = reinterpret_cast<float *>(data);

    const int sampleSize = 4;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position);
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float out = soundData[samplePosition] * volume;
        fbuffer[(sample << 1)] = out;
        fbuffer[(sample << 1) + 1] = out;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromStereo8(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    unsigned char *soundData = reinterpret_cast<unsigned char *>(data);

    const int sampleSize = 1;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position) << 1;
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float outL = ((float)((char)(soundData[samplePosition] - 128))) / (float)((unsigned char)0x80) * volume;
        float outR = ((float)((char)(soundData[samplePosition + 1] - 128))) / (float)((unsigned char)0x80) * volume;
        fbuffer[(sample << 1)] = outL;
        fbuffer[(sample << 1) + 1] = outR;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromStereo16(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    short *soundData = reinterpret_cast<short *>(data);

    const int sampleSize = 2;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position) << 1;
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float outL = (((float)soundData[samplePosition]) / (float)((unsigned int)0x8000)) * volume;
        float outR = (((float)soundData[samplePosition + 1]) / (float)((unsigned int)0x8000)) * volume;
        fbuffer[(sample << 1)] = outL;
        fbuffer[(sample << 1) + 1] = outR;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromStereo32(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    int *soundData = reinterpret_cast<int *>(data);

    const int sampleSize = 4;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position) << 1;
        if (dataSize <= samplePosition * sampleSize)
            return true;

        float outL = ((float)soundData[samplePosition]) / (float)((unsigned int)0x80000000) * volume;
        float outR = ((float)soundData[samplePosition + 1]) / (float)((unsigned int)0x80000000) * volume;
        fbuffer[(sample << 1)] = outL;
        fbuffer[(sample << 1) + 1] = outR;
        position += step;
        sample++;
    }
    return false;
}

// returns bool isFinished
inline bool fillFloatStereoBufferFromStereo32Float(unsigned char *data, int dataSize, float &position, int &sample, float step, float *buffer, int lengthInSamples, float volume)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    float *soundData = reinterpret_cast<float *>(data);

    const int sampleSize = 4;
    while (sample < lengthInSamples)
    {
        // track finished
        int samplePosition = static_cast<int>(position) << 1;
        if (dataSize <= samplePosition * sampleSize)
            return true;

        fbuffer[(sample << 1)] = soundData[samplePosition] * volume;
        fbuffer[(sample << 1) + 1] = soundData[samplePosition + 1] * volume;
        position += step;
        sample++;
    }
    return false;
}