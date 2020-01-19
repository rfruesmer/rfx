#pragma once

#include "rfx/graphics/buffer/Buffer.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(
        uint32_t vertexCount, 
        const VertexFormat& vertexFormat,
        VkDevice vkDevice,
        VkBuffer vkBuffer,
        VkDeviceMemory vkDeviceMemory,
        const VulkanDeviceFunctionPtrs& vk,
        size_t size,
        size_t range);

    uint32_t getVertexCount() const;
    VertexFormat getVertexFormat() const;

private:
    uint32_t vertexCount = 0;
    VertexFormat vertexFormat;
};
    
}
