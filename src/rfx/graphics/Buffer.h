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

    void load(size_t size, const void* inData) const;
    void save(size_t size, void* outData) const;

    [[nodiscard]] VkBuffer getHandle() const;
    [[nodiscard]] VkDeviceMemory getDeviceMemory() const;
    [[nodiscard]] VkDeviceSize getSize() const;

    const VkDescriptorBufferInfo& getDescriptorBufferInfo() const;

private:
    VkDeviceSize size_ = 0;
    VkDevice device_ = VK_NULL_HANDLE;
    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory deviceMemory_ = VK_NULL_HANDLE;
    VkDescriptorBufferInfo descriptorBufferInfo_{};
};

using BufferPtr = std::shared_ptr<Buffer>;

} // namespace rfx
