#include "rfx/pch.h"
#include "rfx/graphics/Texture2D.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture2D::Texture2D(VkDevice device,
    VkImage image, 
    VkDeviceMemory imageMemory, 
    VkImageView imageView, 
    VkImageLayout imageLayout,
    VkSampler sampler,
    const VulkanDeviceFunctionPtrs& vk)
        : vkDevice(device),
          image(image),
          imageMemory(imageMemory),
          imageView(imageView),
          sampler(sampler)
{
    descImageInfo = {
        sampler,
        imageView,
        imageLayout
    };

    vkDestroySampler = vk.vkDestroySampler;
    vkDestroyImage = vk.vkDestroyImage;
    vkDestroyImageView = vk.vkDestroyImageView;
    vkFreeMemory = vk.vkFreeMemory;
}

// ---------------------------------------------------------------------------------------------------------------------

Texture2D::~Texture2D()
{
    dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2D::dispose()
{
    if (sampler) {
        vkDestroySampler(vkDevice, sampler, nullptr);
        sampler = nullptr;
    }

    if (imageView) {
        vkDestroyImageView(vkDevice, imageView, nullptr);
        imageView = nullptr;
    }

    if (image) {
        vkDestroyImage(vkDevice, image, nullptr);
        image = nullptr;
    }

    if (imageMemory) {
        vkFreeMemory(vkDevice, imageMemory, nullptr);
        imageMemory = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const VkDescriptorImageInfo& Texture2D::getDescriptorImageInfo() const
{
    return descImageInfo;
}

// ---------------------------------------------------------------------------------------------------------------------
