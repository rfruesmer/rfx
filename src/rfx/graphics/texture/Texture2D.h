#pragma once

#include "rfx/graphics/texture/Image.h"

namespace rfx
{

class Texture2D
{
public:
    Texture2D(VkDevice device,
        const std::shared_ptr<Image>& image, 
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
    std::shared_ptr<Image> image;
    VkImageView imageView = nullptr;
    VkDescriptorImageInfo descImageInfo = {};
    VkSampler sampler = nullptr;
};
    
}
