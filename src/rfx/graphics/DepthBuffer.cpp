#include "rfx/pch.h"
#include "rfx/graphics/DepthBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

DepthBuffer::DepthBuffer(VkDevice device, shared_ptr<Image> image, VkImageView imageView)
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
    return image->getDesc().format;
}

// ---------------------------------------------------------------------------------------------------------------------

VkImageView DepthBuffer::getImageView() const
{
    return imageView;
}

// ---------------------------------------------------------------------------------------------------------------------

