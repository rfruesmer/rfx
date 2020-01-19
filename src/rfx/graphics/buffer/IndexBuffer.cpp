#include "rfx/pch.h"
#include "rfx/graphics/buffer/IndexBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

IndexBuffer::IndexBuffer(
    uint32_t indexCount, 
    VkIndexType indexFormat, 
    VkDevice vkDevice, 
    VkBuffer vkBuffer,
    VkDeviceMemory vkDeviceMemory, 
    const VulkanDeviceFunctionPtrs& vk, 
    size_t size, 
    size_t range)
        : Buffer(vkDevice, vkBuffer, vkDeviceMemory, vk, size, range),
          indexCount(indexCount),
          indexFormat(indexFormat) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t IndexBuffer::getIndexCount() const
{
    return indexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

VkIndexType IndexBuffer::getIndexFormat() const
{
    return indexFormat;
}

// ---------------------------------------------------------------------------------------------------------------------
