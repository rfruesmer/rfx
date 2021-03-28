#pragma once

#include "rfx/graphics/Buffer.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx {

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(
        uint32_t vertexCount,
        const VertexFormat& vertexFormat,
        VkDevice device,
        VkBuffer buffer,
        VkDeviceMemory deviceMemory);

private:
    uint32_t vertexCount;
    VertexFormat vertexFormat;
};

using VertexBufferPtr = std::shared_ptr<VertexBuffer>;

} // namespace rfx
