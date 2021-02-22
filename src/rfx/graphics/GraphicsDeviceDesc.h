#pragma once

#include "rfx/graphics/QueueFamilyDesc.h"

namespace rfx {

struct GraphicsDeviceDesc
{
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties {};
    VkPhysicalDeviceMemoryProperties memoryProperties {};
    VkPhysicalDeviceFeatures features {};
    std::vector<VkExtensionProperties> extensions;
    std::vector<QueueFamilyDesc> queueFamilies;
    VkSampleCountFlagBits maxSampleCount = VK_SAMPLE_COUNT_1_BIT;
};

} // namespace rfx