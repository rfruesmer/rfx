#pragma once

namespace rfx
{

class Image
{
public:
    explicit Image(VkDevice vkDevice,
        VkImage vkImage, 
        VkDeviceMemory vkDeviceMemory,
        const VulkanDeviceFunctionPtrs& vk);
    ~Image();

    VkImage getHandle() const;

private:
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);
    DECLARE_VULKAN_FUNCTION(vkDestroyImage);

    VkDevice vkDevice = nullptr;
    VkImage vkImage = nullptr;
    VkDeviceMemory vkDeviceMemory = nullptr;
};
    
}
