#pragma once
#include "renderer/vulkan/vulkanPhysicalDevice.h"

// Logical device is needed to interract with physical device

class VulkanLogicalDevice
{
public:
    VulkanLogicalDevice(VulkanPhysicalDevice *physicalDevice);
    ~VulkanLogicalDevice();

    inline bool isReady() { return bIsReady; }

protected:
    VulkanPhysicalDevice *physicalDevice;
    void *logicalDevice = nullptr;
    bool bIsReady = false;
    
    void *graphicsQueue = nullptr;
};