#pragma once

namespace rfx {

struct ImageDesc
{
    VkFormat format = VK_FORMAT_UNDEFINED;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 0;
    uint32_t bytesPerPixel = 0;
    std::vector<VkDeviceSize> mipOffsets;
};

} // namespace rfx
