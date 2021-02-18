#pragma once

#include "rfx/graphics/Image.h"

namespace rfx {

class DepthBuffer
{
public:
    DepthBuffer(VkDevice device, std::shared_ptr<Image> image, VkImageView imageView);
    ~DepthBuffer();

    [[nodiscard]] VkFormat getFormat() const;
    [[nodiscard]] VkImageView getImageView() const;

private:
    VkDevice device;
    std::shared_ptr<Image> image;
    VkImageView imageView;
};

} // namespace rfx
