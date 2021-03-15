#include "rfx/pch.h"
#include "rfx/graphics/Image.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

Image::Image(
    string id,
    ImageDesc desc,
    VkDevice device,
    VkImage image,
    VkDeviceMemory deviceMemory)
        : id(move(id)),
          desc(move(desc)),
          device(device),
          image(image),
          deviceMemory(deviceMemory) {}

// ---------------------------------------------------------------------------------------------------------------------

Image::~Image()
{
    vkFreeMemory(device, deviceMemory, nullptr);
    vkDestroyImage(device, image, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

const string& Image::getId() const
{
    return id;
}

// ---------------------------------------------------------------------------------------------------------------------

VkImage Image::getHandle() const
{
    return image;
}

// ---------------------------------------------------------------------------------------------------------------------

const ImageDesc& Image::getDesc() const
{
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------
