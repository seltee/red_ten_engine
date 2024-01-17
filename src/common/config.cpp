// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "config.h"
#include <stdio.h>

#define BUFFER_SIZE 2048

bool Config::loadConfig()
{
    std::vector<ConfigPair> configPairs;
    char buffer[BUFFER_SIZE];
    FILE *cfgFile = fopen(configFilePath.c_str(), "r");

    if (cfgFile != nullptr)
    {
        while (fgets(buffer, BUFFER_SIZE, cfgFile) != NULL)
        {
            ConfigPair pair;
            if (getPairFromString(buffer, BUFFER_SIZE, &pair))
                configPairs.push_back(pair);
        }

        if (!feof(cfgFile))
        {
            fclose(cfgFile);
            return false;
        }

        for (auto it = configPairs.begin(); it != configPairs.end(); it++)
        {
            if (it->name == "windowWidth" && std::stoi(it->value))
                windowWidth = std::stoi(it->value);
            if (it->name == "windowHeight" && std::stoi(it->value))
                windowHeight = std::stoi(it->value);
            if (it->name == "fullscreen")
                bIsFullscreen = it->value == "true";
            if (it->name == "videoDevice")
                videoDevice = it->value;
            if (it->name == "audioDevice")
                audioDevice = it->value;
            if (it->name == "shadowQuality")
                shadowQuality = stringToQuality(it->value);
            if (it->name == "antialiasing")
                antialiasing = stringToAntialiasing(it->value);
        }

        bIsLoaded = true;
        fclose(cfgFile);
        return true;
    }
    logger->logff("No config file found\n");
    return false;
}

bool Config::saveConfig()
{
    std::vector<ConfigPair> configPairs;
    char buffer[2048];
    FILE *cfgFile = fopen(configFilePath.c_str(), "w");

    if (cfgFile != nullptr)
    {
        sprintf(buffer, "%s=%d\n", "windowWidth", windowWidth);
        fputs(buffer, cfgFile);
        sprintf(buffer, "%s=%d\n", "windowHeight", windowHeight);
        fputs(buffer, cfgFile);
        sprintf(buffer, "%s=%s\n", "fullscreen", bIsFullscreen ? "true" : "false");
        fputs(buffer, cfgFile);
        sprintf(buffer, "%s=%s\n", "videoDevice", videoDevice.c_str());
        fputs(buffer, cfgFile);
        sprintf(buffer, "%s=%s\n", "audioDevice", audioDevice.c_str());
        fputs(buffer, cfgFile);
        sprintf(buffer, "%s=%s\n", "shadowQuality", qualityToString(shadowQuality).c_str());
        fputs(buffer, cfgFile);
        sprintf(buffer, "%s=%s\n", "antialiasing", antialiasingToString(antialiasing).c_str());
        fputs(buffer, cfgFile);

        fclose(cfgFile);
        return true;
    }
    else
        logger->logff("Error happend on conf file open\n");

    return false;
}

void Config::setConfigFilePath(std::string configFilePath)
{
    this->configFilePath = configFilePath;
}

std::string Config::getConfigFilePath()
{
    return configFilePath;
}

void Config::setupByQuality(RenderQuality quality)
{
    shadowQuality = quality;
    antialiasing = quality == RenderQuality::High ? AntiAliasing::SSAA : AntiAliasing::None;
}

bool Config::isLoaded()
{
    return bIsLoaded;
}

void Config::setCurrentVideoDevice(std::string deviceName)
{
    this->videoDevice = deviceName;
}

std::string Config::getCurrentVideoDevice()
{
    return videoDevice;
}

void Config::setCurrentAudioDevice(std::string deviceName)
{
    audioDevice = deviceName;
}

std::string Config::getCurrentAudioDevice()
{
    return audioDevice;
}

int Config::getWindowWidth()
{
    return windowWidth;
}

void Config::setWindowWidth(int width)
{
    if (this->windowWidth != width)
        this->windowWidth = width;
}

int Config::getWindowHeight()
{
    return windowHeight;
}

void Config::setWindowHeight(int height)
{
    if (this->windowHeight != height)
        this->windowHeight = height;
}

int Config::getRefreshRate()
{
    return refreshRate;
}

void Config::setRefreshRate(int refreshRate)
{
    this->refreshRate = refreshRate;
}

bool Config::isFullscreen()
{
    return bIsFullscreen;
}

void Config::setFullscreenState(bool isFullscreen)
{
    this->bIsFullscreen = isFullscreen;
}

RenderQuality Config::getShadowQuality()
{
    return shadowQuality;
}

void Config::setShadowQuality(RenderQuality quality)
{
    if (this->shadowQuality != quality)
        this->shadowQuality = quality;
}

AntiAliasing Config::getAnialiasing()
{
    return antialiasing;
}

void Config::setAnialiasing(AntiAliasing state)
{
    if (this->antialiasing != state)
        this->antialiasing = state;
}

float Config::getMultisamplingFactor()
{
    switch (this->antialiasing)
    {
    case AntiAliasing::SSAA:
        return 1.25f;
    case AntiAliasing::SSAA2:
        return 1.5f;
    case AntiAliasing::SSAA3:
        return 1.75f;
    case AntiAliasing::SSAA4:
        return 2.0f;
    default:
        return 1.0f;
    }
}

bool Config::isSameWith(Config *otherConfig)
{
    bool same = true;
    same &= getWindowWidth() == otherConfig->getWindowWidth();
    same &= getWindowHeight() == otherConfig->getWindowHeight();
    same &= isFullscreen() == otherConfig->isFullscreen();
    same &= getRefreshRate() == otherConfig->getRefreshRate();
    same &= getShadowQuality() == otherConfig->getShadowQuality();
    same &= getAnialiasing() == otherConfig->getAnialiasing();
    same &= getCurrentVideoDevice() == otherConfig->getCurrentVideoDevice();
    same &= getCurrentAudioDevice() == otherConfig->getCurrentAudioDevice();
    return same;
}

void Config::copyFrom(Config *otherConfig)
{
    setWindowWidth(otherConfig->getWindowWidth());
    setWindowHeight(otherConfig->getWindowHeight());
    setFullscreenState(otherConfig->isFullscreen());
    setRefreshRate(otherConfig->getRefreshRate());
    setShadowQuality(otherConfig->getShadowQuality());
    setAnialiasing(otherConfig->getAnialiasing());
    setCurrentVideoDevice(otherConfig->getCurrentVideoDevice());
    setCurrentAudioDevice(otherConfig->getCurrentAudioDevice());
}

std::string Config::qualityToString(RenderQuality quality)
{
    if (quality == RenderQuality::SuperFast)
        return "super fast";
    if (quality == RenderQuality::Fast)
        return "fast";
    if (quality == RenderQuality::High)
        return "high";
    return "balanced";
}

RenderQuality Config::stringToQuality(std::string quality)
{
    if (quality == "super fast")
        return RenderQuality::SuperFast;
    if (quality == "fast")
        return RenderQuality::Fast;
    if (quality == "high")
        return RenderQuality::High;
    return RenderQuality::Balanced;
}

std::string Config::antialiasingToString(AntiAliasing quality)
{
    if (quality == AntiAliasing::None)
        return "none";
    if (quality == AntiAliasing::FXAA)
        return "fxaa";
    if (quality == AntiAliasing::SSAA)
        return "ssaa";
    if (quality == AntiAliasing::SSAA2)
        return "ssaa2";
    if (quality == AntiAliasing::SSAA3)
        return "ssaa3";
    if (quality == AntiAliasing::SSAA4)
        return "ssaa4";
    return "none";
}

AntiAliasing Config::stringToAntialiasing(std::string quality)
{
    if (quality == "none")
        return AntiAliasing::None;
    if (quality == "fxaa")
        return AntiAliasing::FXAA;
    if (quality == "ssaa")
        return AntiAliasing::SSAA;
    if (quality == "ssaa2")
        return AntiAliasing::SSAA2;
    if (quality == "ssaa3")
        return AntiAliasing::SSAA3;
    if (quality == "ssaa4")
        return AntiAliasing::SSAA4;
    return AntiAliasing::None;
}

bool Config::getPairFromString(char *buffer, int limit, ConfigPair *pair)
{
    if (strlen(buffer) >= limit)
        return false;

    int iterator = 0;
    bool readingValue = false;
    while (iterator < limit && buffer[iterator] != 0 && buffer[iterator] != 0 && buffer[iterator] != '\n')
    {
        if (buffer[iterator] == '=')
        {
            if (readingValue)
                return false;
            readingValue = true;
            iterator++;
            continue;
        }
        if (readingValue)
            pair->value += buffer[iterator];
        else
            pair->name += buffer[iterator];
        iterator++;
    }
    return true;
}
