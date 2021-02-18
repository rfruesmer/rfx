#pragma once

#include "rfx/graphics/Image.h"

namespace rfx {

class Texture2D
{
public:
    Texture2D(
        VkDevice device,
        std::shared_ptr<Image> image,
        VkImageView imageView,
        VkImageLayout imageLayout,
        VkSampler sampler);

    ~Texture2D();

    [[nodiscard]] VkImageView getImageView() const;
    [[nodiscard]] VkSampler getSampler() const;

private:
    VkDevice device;
    std::shared_ptr<Image> image;
    VkImageView imageView;
    VkImageLayout imageLayout;
    VkSampler sampler;
};

} // namespace rfx