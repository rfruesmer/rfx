#pragma once

namespace rfx
{

class Texture2D
{
public:
    Texture2D(VkDevice device,
        VkImage image, 
        VkDeviceMemory imageMemory, 
        VkImageView imageView, 
        VkImageLayout imageLayout,
        VkSampler sampler,
        const VulkanDeviceFunctionPtrs& vk);

    ~Texture2D();

    void dispose();

    const VkDescriptorImageInfo& getDescriptorImageInfo() const;

private:
    DECLARE_VULKAN_FUNCTION(vkDestroySampler);
    DECLARE_VULKAN_FUNCTION(vkDestroyImage);
    DECLARE_VULKAN_FUNCTION(vkDestroyImageView);
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);

    VkDevice vkDevice = nullptr;
    VkImage image = nullptr;
    VkDeviceMemory imageMemory = nullptr;
    VkImageView imageView = nullptr;
    VkDescriptorImageInfo descImageInfo = {};
    VkSampler sampler = nullptr;
};
    
}
