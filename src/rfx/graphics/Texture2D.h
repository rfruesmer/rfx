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

    [[nodiscard]] const std::shared_ptr<Image>& getImage() const;
    [[nodiscard]] VkImageView getImageView() const;
    [[nodiscard]] VkSampler getSampler() const;
    [[nodiscard]] const VkDescriptorImageInfo& getDescriptorImageInfo() const;

private:
    VkDevice device;
    std::shared_ptr<Image> image;
    VkImageView imageView;
    VkImageLayout imageLayout;
    VkSampler sampler;
    VkDescriptorImageInfo descriptorImageInfo;
};

using Texture2DPtr = std::shared_ptr<Texture2D>;

} // namespace rfx