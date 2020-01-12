#pragma once

namespace rfx
{

class Buffer
{
public:
    Buffer(VkDevice vkDevice, 
        VkBuffer vkBuffer, 
        VkDeviceMemory vkDeviceMemory, 
        const VulkanDeviceFunctionPtrs& vk, 
        size_t size,
        size_t range);

    ~Buffer();

    void dispose();

    VkBuffer getHandle() const;

    void load(size_t size, const std::byte* data) const;
    void bind() const;
    void invalidateMappedMemoryRanges() const;

    const VkDescriptorBufferInfo& getBufferInfo() const;
    VkDeviceMemory getDeviceMemory() const;

private:
    VkDevice vkDevice = nullptr;
    VkBuffer vkBuffer = nullptr;
    VkDeviceMemory vkDeviceMemory = nullptr;
    VkDescriptorBufferInfo vkBufferInfo = {};
    size_t size = 0;

    DECLARE_VULKAN_FUNCTION(vkMapMemory);
    DECLARE_VULKAN_FUNCTION(vkUnmapMemory);
    DECLARE_VULKAN_FUNCTION(vkBindBufferMemory);
    DECLARE_VULKAN_FUNCTION(vkInvalidateMappedMemoryRanges);
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);
    DECLARE_VULKAN_FUNCTION(vkDestroyBuffer);
};
    
} // namespace rfx
