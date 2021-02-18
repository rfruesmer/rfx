#include "rfx/pch.h"
#include "rfx/graphics/IndexBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

IndexBuffer::IndexBuffer(
    uint32_t indexCount,
    const VkIndexType& indexType,
    VkDeviceSize size,
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory deviceMemory)
        : Buffer(size, device, buffer, deviceMemory),
          indexCount(indexCount),
          indexType(indexType) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t IndexBuffer::getIndexCount() const
{
    return indexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

VkIndexType IndexBuffer::getIndexType() const
{
    return indexType;
}

// ---------------------------------------------------------------------------------------------------------------------
