#pragma once
#include "renderer/renderer.h"
#include "connector/withLogger.h"
#include "renderer/vulkan/vulkanPhysicalDevice.h"
#include "renderer/vulkan/vulkanLogicalDevice.h"


class RendererVulkan : public Renderer, public WithLogger
{
public:
    ~RendererVulkan();

    static bool isAvailable();

    // called before window creation
    void preInit() final override;

    // called after window creation
    bool init(void *window) final override;

    unsigned int getWindowFlags() final override;

protected:
    bool checkExtensions();
    bool createInstance();

    void *instance = nullptr;
    VulkanPhysicalDevice *physicalDevice = nullptr;
    VulkanLogicalDevice *logicalDevice = nullptr;
};