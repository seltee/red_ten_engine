// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "connector/withLogger.h"
#include "common/utils.h"
#include <string>
#include <vector>

enum class RenderQuality
{
    SuperFast = 0,
    Fast,
    Balanced,
    High
};

enum class AntiAliasing{
    None = 0,
    FXAA,
    SSAA,
    SSAA2,
    SSAA3,
    SSAA4,
};

struct ConfigPair
{
    std::string name;
    std::string value;
};


class Config : public WithLogger
{
public:
    EXPORT bool loadConfig();
    EXPORT bool saveConfig();
    EXPORT void setConfigFilePath(std::string configFilePath);
    EXPORT std::string getConfigFilePath();
    EXPORT void setupByQuality(RenderQuality quality);

    EXPORT bool isLoaded();

    EXPORT void setCurrentVideoDevice(std::string deviceName);
    EXPORT std::string getCurrentVideoDevice();
    EXPORT void setCurrentAudioDevice(std::string deviceName);
    EXPORT std::string getCurrentAudioDevice();

    EXPORT int getWindowWidth();
    EXPORT void setWindowWidth(int width);
    EXPORT int getWindowHeight();
    EXPORT void setWindowHeight(int height);
    EXPORT int getRefreshRate();
    EXPORT void setRefreshRate(int refreshRate);
    EXPORT bool isFullscreen();
    EXPORT void setFullscreenState(bool isFullscreen);
    EXPORT RenderQuality getShadowQuality();
    EXPORT void setShadowQuality(RenderQuality quality);
    EXPORT AntiAliasing getAnialiasing();
    EXPORT void setAnialiasing(AntiAliasing state);
    EXPORT float getMultisamplingFactor();

    EXPORT bool isSameWith(Config *otherConfig);
    EXPORT void copyFrom(Config *otherConfig);

    EXPORT static std::string qualityToString(RenderQuality quality);
    EXPORT static RenderQuality stringToQuality(std::string quality);
    EXPORT static std::string antialiasingToString(AntiAliasing quality);
    EXPORT static AntiAliasing stringToAntialiasing(std::string quality);

protected:
    bool getPairFromString(char *buffer, int limit, ConfigPair *pair);

    std::string configFilePath = "./cfg";
    bool bIsLoaded = false;

    int windowWidth = 0, windowHeight = 0;
    int refreshRate = 0;
    bool bIsFullscreen = false;
    std::string videoDevice = "";
    std::string audioDevice = "";

    RenderQuality shadowQuality = RenderQuality::High;
    AntiAliasing antialiasing = AntiAliasing::None;
};
