#include "renderer/vulkan/vulkanLogicalDevice.h"
#include "renderer/vulkan/vulkan/vulkan.h"
#include <SDL.h>
#include <vector>

VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice *physicalDevice)
{
    this->physicalDevice = physicalDevice;
    VkDevice device;

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = physicalDevice->getGraphicsFamilyIndex();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice((VkPhysicalDevice)physicalDevice->getPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS)
        return;

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, physicalDevice->getGraphicsFamilyIndex(), 0, &graphicsQueue);
    this->graphicsQueue = graphicsQueue;

    this->logicalDevice = device;
    bIsReady = true;
}

VulkanLogicalDevice::~VulkanLogicalDevice()
{
    if (logicalDevice)
        vkDestroyDevice((VkDevice)logicalDevice, nullptr);
}