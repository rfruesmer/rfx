#include "rfx/pch.h"
#include "rfx/graphics/DepthBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

DepthBuffer::DepthBuffer(VkDevice device, std::shared_ptr<Image> image, VkImageView imageView)
    : device(device),
      image(move(image)),
      imageView(imageView) {}

// ---------------------------------------------------------------------------------------------------------------------

DepthBuffer::~DepthBuffer()
{
    vkDestroyImageView(device, imageView, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

VkFormat DepthBuffer::getFormat() const
{
    return image->getFormat();
}

// ---------------------------------------------------------------------------------------------------------------------

VkImageView DepthBuffer::getImageView() const
{
    return imageView;
}

// ---------------------------------------------------------------------------------------------------------------------

