#include "rfx/pch.h"
#include "rfx/graphics/VertexBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexBuffer::VertexBuffer(
    uint32_t vertexCount,
    const VertexFormat& vertexFormat, 
    VkDevice vkDevice,
    VkBuffer vkBuffer, 
    VkDeviceMemory vkDeviceMemory,
    const VulkanDeviceFunctionPtrs& vk, 
    size_t size, 
    size_t range)
        : Buffer(vkDevice, vkBuffer, vkDeviceMemory, vk, size, range),
          vertexCount(vertexCount),
          vertexFormat(vertexFormat) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexBuffer::getVertexCount() const
{
    return vertexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat VertexBuffer::getVertexFormat() const
{
    return vertexFormat;
}

// ---------------------------------------------------------------------------------------------------------------------

