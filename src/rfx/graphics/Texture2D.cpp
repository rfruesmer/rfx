#include "rfx/pch.h"
#include "rfx/graphics/Texture2D.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture2D::Texture2D(
    VkDevice device,
    shared_ptr<Image> image,
    VkImageView imageView,
    VkImageLayout imageLayout,
    VkSampler sampler)
        : device(device),
          image(move(image)),
          imageView(imageView),
          imageLayout(imageLayout),
          sampler(sampler) {}

// ---------------------------------------------------------------------------------------------------------------------

Texture2D::~Texture2D()
{
    vkDestroySampler(device, sampler, nullptr);
    vkDestroyImageView(device, imageView, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Image>& Texture2D::getImage() const
{
    return image;
}

// ---------------------------------------------------------------------------------------------------------------------

VkImageView Texture2D::getImageView() const
{
    return imageView;
}

// ---------------------------------------------------------------------------------------------------------------------

VkSampler Texture2D::getSampler() const
{
    return sampler;
}

// ---------------------------------------------------------------------------------------------------------------------
