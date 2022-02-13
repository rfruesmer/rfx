#pragma once

namespace rfx {

struct ImageDesc
{
    VkFormat format = VK_FORMAT_UNDEFINED;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t bytesPerPixel = 0;
    uint32_t channels = 0;
    uint32_t layers = 1;
    uint32_t mipLevels = 0;
    std::vector<VkDeviceSize> mipOffsets;
    bool isCubemap = false;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
};

} // namespace rfx
