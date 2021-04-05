#include "rfx/pch.h"
#include "rfx/graphics/Texture.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture::Texture(
    VkDevice device,
    ImagePtr image,
    VkImageView imageView,
    VkImageLayout imageLayout,
    VkSampler sampler)
        : device(device),
          image(move(image)),
          imageView(imageView),
          imageLayout(imageLayout),
          sampler(sampler)
{
    RFX_CHECK_ARGUMENT(imageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    descriptorImageInfo = {
        .sampler = sampler,
        .imageView = imageView,
        .imageLayout = imageLayout
    };
}

// ---------------------------------------------------------------------------------------------------------------------

Texture::~Texture()
{
    vkDestroySampler(device, sampler, nullptr);
    vkDestroyImageView(device, imageView, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

const ImagePtr& Texture::getImage() const
{
    return image;
}

// ---------------------------------------------------------------------------------------------------------------------

VkImageView Texture::getImageView() const
{
    return imageView;
}

// ---------------------------------------------------------------------------------------------------------------------

VkSampler Texture::getSampler() const
{
    return sampler;
}

// ---------------------------------------------------------------------------------------------------------------------

const VkDescriptorImageInfo& Texture::getDescriptorImageInfo() const
{
    return descriptorImageInfo;
}

// ---------------------------------------------------------------------------------------------------------------------
