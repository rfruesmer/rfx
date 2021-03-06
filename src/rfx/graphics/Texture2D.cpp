#include "rfx/pch.h"
#include "rfx/graphics/Texture2D.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture2D::Texture2D(
    VkDevice device,
    ImagePtr image,
    VkImageView imageView,
    VkImageLayout imageLayout,
    VkSampler sampler)
        : Texture(
            device,
            image,
            imageView,
            imageLayout,
            sampler) {}

// ---------------------------------------------------------------------------------------------------------------------

