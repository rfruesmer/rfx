#include "rfx/pch.h"
#include "rfx/graphics/VertexBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexBuffer::VertexBuffer(
    uint32_t vertexCount,
    const VertexFormat& vertexFormat,
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory deviceMemory)
        : Buffer(vertexCount * vertexFormat.getVertexSize(), device, buffer, deviceMemory),
          vertexCount(vertexCount),
          vertexFormat(vertexFormat) {}

// ---------------------------------------------------------------------------------------------------------------------
