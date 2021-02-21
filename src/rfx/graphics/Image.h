#pragma once

#include "rfx/graphics/ImageDesc.h"

namespace rfx {

class Image
{
public:
    Image(
        ImageDesc desc,
        VkDevice device,
        VkImage image,
        VkDeviceMemory deviceMemory);

    ~Image();

    [[nodiscard]] VkImage getHandle() const;
    [[nodiscard]] const ImageDesc& getDesc() const;

private:
    ImageDesc desc;
    VkImage image;
    VkDevice device;
    VkDeviceMemory deviceMemory;
};

} // namespace rfx