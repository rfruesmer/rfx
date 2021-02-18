#pragma once

namespace rfx {

class Image
{
public:
    Image(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkDevice device,
        VkImage image,
        VkDeviceMemory deviceMemory);

    ~Image();

    [[nodiscard]] VkImage getHandle() const;
    [[nodiscard]] uint32_t getWidth() const;
    [[nodiscard]] uint32_t getHeight() const;
    [[nodiscard]] VkFormat getFormat() const;

private:
    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkDevice device;
    VkImage image;
    VkDeviceMemory deviceMemory;
};

} // namespace rfx