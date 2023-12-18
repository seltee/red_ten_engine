#include "renderer/vulkan/rendererVulkan.h"
#include "renderer/vulkan/vulkan/vulkan.h"
#include <SDL.h>
#include <vector>
#include <string.h>

RendererVulkan::~RendererVulkan()
{
    if (physicalDevice)
        delete physicalDevice;

    if (logicalDevice)
        delete logicalDevice;

    if (instance)
        vkDestroyInstance((VkInstance)instance, nullptr);
}

bool RendererVulkan::isAvailable()
{
    return false;
}

void RendererVulkan::preInit()
{
}

bool RendererVulkan::init(void *window)
{
    logger->logff("Initializing Vulkan ...");

    if (!checkExtensions())
    {
        logger->logff("Mandatory extensions are missing");
        return false;
    }

    if (!createInstance())
    {
        logger->logff("Failed to create Vulkan instance");
        return false;
    }

    physicalDevice = new VulkanPhysicalDevice(instance);
    if (!physicalDevice->isReady())
    {
        logger->logff("Failed to initialize physical device");
        return false;
    }

    logicalDevice = new VulkanLogicalDevice(physicalDevice);
    if (!logicalDevice->isReady())
    {
        logger->logff("Failed to initialize logival device");
        return false;
    }

    return false;
}

unsigned int RendererVulkan::getWindowFlags()
{
    return SDL_WINDOW_VULKAN;
}

bool RendererVulkan::checkExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    logger->logConsole("Available extensions:");

    bool surfaceExtensionFound = false;

    for (const auto &extension : extensions)
    {
        logger->logConsole("- %s:", extension.extensionName);
        if (strcmp("VK_KHR_surface", extension.extensionName) == 0)
            surfaceExtensionFound = true;
    }

    return surfaceExtensionFound;
}

bool RendererVulkan::createInstance()
{
    VkInstance instance;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Red Ten Engine with Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Red Ten Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;

    const int extensionsCount = 1;
    const char *extensions[1] = {"VK_KHR_surface"};

    createInfo.enabledExtensionCount = extensionsCount;
    createInfo.ppEnabledExtensionNames = extensions;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result == VK_SUCCESS)
    {
        logger->logff("Vulkan instance created");
        this->instance = instance;
        return true;
    }
    return false;
}