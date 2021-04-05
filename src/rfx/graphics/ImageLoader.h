#pragma once

#include "rfx/graphics/ImageDesc.h"

namespace rfx {

class ImageLoader
{
public:
    void load(
        const std::filesystem::path& imagePath,
        ImageDesc* outImageDesc,
        std::vector<std::byte>* outImageData) const;
};

} // namespace rfx
