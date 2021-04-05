#pragma once

#include "rfx/graphics/Image.h"


namespace rfx {

class Texture
{
public:
    Texture(
        VkDevice device,
        ImagePtr image,
        VkImageView imageView,
        VkImageLayout imageLayout,
        VkSampler sampler);

    virtual ~Texture();

    [[nodiscard]] const ImagePtr& getImage() const;
    [[nodiscard]] VkImageView getImageView() const;
    [[nodiscard]] VkSampler getSampler() const;
    [[nodiscard]] const VkDescriptorImageInfo& getDescriptorImageInfo() const;

private:
    VkDevice device;
    ImagePtr image;
    VkImageView imageView;
    VkImageLayout imageLayout;
    VkSampler sampler;
    VkDescriptorImageInfo descriptorImageInfo {};
};

using TexturePtr = std::shared_ptr<Texture>;

} // namespace rfx
