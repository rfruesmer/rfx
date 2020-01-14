#include "rfx/pch.h"
#include "rfx/graphics/Image.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Image::Image(VkDevice vkDevice,
    VkImage vkImage, 
    VkDeviceMemory vkDeviceMemory,
    const VulkanDeviceFunctionPtrs& vk)
        : vkDevice(vkDevice),
          vkImage(vkImage),
          vkDeviceMemory(vkDeviceMemory)
{
    vkFreeMemory = vk.vkFreeMemory;
    vkDestroyImage = vk.vkDestroyImage;
}

// ---------------------------------------------------------------------------------------------------------------------

Image::~Image()
{
    if (vkImage) {
        vkDestroyImage(vkDevice, vkImage, nullptr);
        vkImage = nullptr;
    }

    if (vkDeviceMemory) {
        vkFreeMemory(vkDevice, vkDeviceMemory, nullptr);
        vkDeviceMemory = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkImage Image::getHandle() const
{
    return vkImage;
}

// ---------------------------------------------------------------------------------------------------------------------
