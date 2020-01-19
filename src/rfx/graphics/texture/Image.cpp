#include "rfx/pch.h"
#include "rfx/graphics/texture/Image.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Image::Image(VkDevice vkDevice,
    VkImage vkImage, 
    VkDeviceMemory vkDeviceMemory,
    uint32_t width,
    uint32_t height,
    const VulkanDeviceFunctionPtrs& vk)
        : vkDevice(vkDevice),
          vkImage(vkImage),
          width(width),
          height(height),
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

uint32_t Image::getWidth() const
{
    return width;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Image::getHeight() const
{
    return height;
}

// ---------------------------------------------------------------------------------------------------------------------
