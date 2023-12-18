#include "renderer/vulkan/vulkanPhysicalDevice.h"
#include "renderer/vulkan/vulkan/vulkan.h"
#include <SDL.h>
#include <vector>

VulkanPhysicalDevice::VulkanPhysicalDevice(void *instance)
{
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    this->instance = instance;
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices((VkInstance)instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        return;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices((VkInstance)instance, &deviceCount, devices.data());

    logger->logConsole("Available vulkan devices:");
    int lastScore = 0;
    for (const auto &device : devices)
    {
        int currentScore = rateDeviceSuitability(device);
        if (currentScore > lastScore)
        {
            physicalDevice = device;
            lastScore = currentScore;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties((VkPhysicalDevice)device, &deviceProperties);
        logger->logConsole("%s (score %i)", deviceProperties.deviceName, currentScore);
    }

    if (physicalDevice == VK_NULL_HANDLE || lastScore == 0)
        return;

    if (!findQueueFamilies(physicalDevice))
        return;

    this->physicalDevice = physicalDevice;
    bIsReady = true;
}

int VulkanPhysicalDevice::rateDeviceSuitability(void *device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties((VkPhysicalDevice)device, &deviceProperties);
    vkGetPhysicalDeviceFeatures((VkPhysicalDevice)device, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 4000;

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader)
        return 0;

    if (!findQueueFamilies((VkPhysicalDevice)device))
        return 0;

    return score;
}

bool VulkanPhysicalDevice::findQueueFamilies(void *device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties((VkPhysicalDevice)device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties((VkPhysicalDevice)device, &queueFamilyCount, queueFamilies.data());

    bool graphicsFamilyFound = false;

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamilyIndex = i;
            graphicsFamilyFound = true;
        }

        i++;
    }

    return graphicsFamilyFound;
}