#pragma once

#include "rfx/graphics/command/QueueFamilyInfo.h"

namespace rfx
{

struct GraphicsDeviceInfo
{
    VkPhysicalDevice handle = nullptr;
    std::vector<VkExtensionProperties> extensions;
    VkPhysicalDeviceFeatures features = {};
    VkPhysicalDeviceProperties properties = {};
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    std::vector<QueueFamilyInfo> queueFamilies;
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;
    bool supportsPresentation = false;
    VkSurfaceKHR presentSurface = nullptr;
    std::vector<VkPresentModeKHR> presentModes;
    //VkSurfaceCapabilitiesKHR presentSurfaceCapabilities = {};
    //std::vector<VkSurfaceFormatKHR> presentSurfaceFormats;
    //uint32_t presentImageCount = 0;
    //VkExtent2D presentImageSize = {};
    VkFormatProperties depthBufferFormatProperties = {};
};

}
