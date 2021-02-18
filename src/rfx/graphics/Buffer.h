#pragma once

namespace rfx {

class Buffer
{
public:
    Buffer(
        VkDeviceSize size,
        VkDevice device,
        VkBuffer buffer,
        VkDeviceMemory deviceMemory);

    virtual ~Buffer();

    [[nodiscard]] VkBuffer getHandle() const;
    [[nodiscard]] VkDeviceMemory getDeviceMemory() const;
    [[nodiscard]] VkDeviceSize getSize() const;

private:
    VkDeviceSize size = 0;
    VkDevice device = VK_NULL_HANDLE;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
};

} // namespace rfx
