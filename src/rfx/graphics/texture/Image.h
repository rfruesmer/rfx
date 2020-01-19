#pragma once

namespace rfx
{

class Image
{
public:
    explicit Image(VkDevice vkDevice,
        VkImage vkImage, 
        VkDeviceMemory vkDeviceMemory,
        uint32_t width,
        uint32_t height,
        const VulkanDeviceFunctionPtrs& vk);
    ~Image();

    VkImage getHandle() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);
    DECLARE_VULKAN_FUNCTION(vkDestroyImage);

    VkDevice vkDevice = nullptr;
    VkImage vkImage = nullptr;
    VkDeviceMemory vkDeviceMemory = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;
};
    
}
