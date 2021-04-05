#pragma once

#include "rfx/graphics/Texture.h"


namespace rfx {

class CubeMap : public Texture
{
public:
    CubeMap(
        VkDevice device,
        ImagePtr image,
        VkImageView imageView,
        VkImageLayout imageLayout,
        VkSampler sampler);
};

using CubeMapPtr = std::shared_ptr<CubeMap>;

} // namespace rfx
