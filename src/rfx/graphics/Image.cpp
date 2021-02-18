#include "rfx/pch.h"
#include "rfx/graphics/Image.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

Image::Image(
    uint32_t width,
    uint32_t height,
    VkDevice device,
    VkImage image,
    VkDeviceMemory deviceMemory)
        : width(width),
          height(height),
          device(device),
          image(image),
          deviceMemory(deviceMemory) {}

// ---------------------------------------------------------------------------------------------------------------------

Image::~Image()
{
    vkDestroyImage(device, image, nullptr);
    vkFreeMemory(device, deviceMemory, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

VkImage Image::getHandle() const
{
    return image;
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
