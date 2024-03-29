#pragma once

#include "rfx/graphics/ImageDesc.h"

namespace rfx {

class Image
{
public:
    Image(
        std::string id,
        ImageDesc desc,
        VkDevice device,
        VkImage image,
        VkDeviceMemory deviceMemory);

    ~Image();

    [[nodiscard]] const std::string& getId() const;
    [[nodiscard]] VkImage getHandle() const;

    void setDesc(const ImageDesc& desc);
    [[nodiscard]] const ImageDesc& getDesc() const;

private:
    std::string id;
    ImageDesc desc;
    VkImage image;
    VkDevice device;
    VkDeviceMemory deviceMemory;
};

using ImagePtr = std::shared_ptr<Image>;

} // namespace rfx