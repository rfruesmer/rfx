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

    void load(size_t size, const void* data) const;

    [[nodiscard]] VkBuffer getHandle() const;
    [[nodiscard]] VkDeviceMemory getDeviceMemory() const;
    [[nodiscard]] VkDeviceSize getSize() const;

private:
    VkDeviceSize size_ = 0;
    VkDevice device_ = VK_NULL_HANDLE;
    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory deviceMemory_ = VK_NULL_HANDLE;
};

} // namespace rfx
