#include "rfx/pch.h"
#include "rfx/graphics/Buffer.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(
    VkDeviceSize size,
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory deviceMemory)
        : size(size),
          device(device),
          buffer(buffer),
          deviceMemory(deviceMemory) {}

// ---------------------------------------------------------------------------------------------------------------------

Buffer::~Buffer()
{
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, deviceMemory, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

VkBuffer Buffer::getHandle() const
{
    return buffer;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDeviceMemory Buffer::getDeviceMemory() const
{
    return deviceMemory;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDeviceSize Buffer::getSize() const
{
    return size;
}

// ---------------------------------------------------------------------------------------------------------------------
