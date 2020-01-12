#pragma once

#include "rfx/graphics/Buffer.h"

namespace rfx
{
    
class IndexBuffer : public Buffer
{
public:
    IndexBuffer(
        uint32_t indexCount,
        VkIndexType indexFormat,
        VkDevice vkDevice,
        VkBuffer vkBuffer,
        VkDeviceMemory vkDeviceMemory,
        const VulkanDeviceFunctionPtrs& vk,
        size_t size,
        size_t range);

    uint32_t getIndexCount() const;
    VkIndexType getIndexFormat() const;

private:
    uint32_t indexCount = 0;
    VkIndexType indexFormat = VK_INDEX_TYPE_UINT32;
};

}

