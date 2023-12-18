#pragma once
#include "connector/withLogger.h"

// Queue families and physical device score based selection

class VulkanPhysicalDevice : public WithLogger
{
public:
    VulkanPhysicalDevice(void *instance);

    inline bool isReady() { return bIsReady; }
    inline void *getPhysicalDevice() { return physicalDevice; }

    inline unsigned int getGraphicsFamilyIndex() { return graphicsFamilyIndex; }

protected:
    int rateDeviceSuitability(void *device);
    bool findQueueFamilies(void *device);

    void *physicalDevice = nullptr;
    void *instance = nullptr;

    bool bIsReady = false;

    unsigned int graphicsFamilyIndex = 0;
};
