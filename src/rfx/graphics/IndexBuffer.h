#pragma once

#include "rfx/graphics/Buffer.h"

namespace rfx {

class IndexBuffer : public Buffer
{
public:
    IndexBuffer(
        uint32_t indexCount,
        const VkIndexType& indexType,
        VkDeviceSize size,
        VkDevice device,
        VkBuffer buffer,
        VkDeviceMemory deviceMemory);

    [[nodiscard]] uint32_t getIndexCount() const;

    [[nodiscard]] VkIndexType getIndexType() const;

private:
    uint32_t indexCount = 0;
    VkIndexType indexType = VK_INDEX_TYPE_UINT32;
};

} // namespace rfx