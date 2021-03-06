#pragma once

#include "rfx/graphics/Texture.h"

namespace rfx {

class Texture2D : public Texture
{
public:
    Texture2D(
        VkDevice device,
        ImagePtr image,
        VkImageView imageView,
        VkImageLayout imageLayout,
        VkSampler sampler);
};

using Texture2DPtr = std::shared_ptr<Texture2D>;

} // namespace rfx