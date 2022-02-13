#pragma once

#include "rfx/graphics/ImageDesc.h"
#include "rfx/graphics/ImageChannelType.h"

namespace rfx {

class ImageLoader
{
public:
    void load(
        const std::filesystem::path& imagePath,
        ImageDesc* outImageDesc,
        std::vector<std::byte>* outImageData) const;

    void load(
        const std::filesystem::path& imagePath,
        ImageChannelType channelType,
        int desiredChannels,
        ImageDesc* outImageDesc,
        std::vector<std::byte>* outImageData) const;

    void load(
        const std::filesystem::path& imagePath,
        int desiredChannels,
        ImageDesc* outImageDesc,
        std::vector<std::byte>* outImageData) const;

    void load(
        const std::filesystem::path& imagePath,
        int desiredChannels,
        ImageDesc* outImageDesc,
        std::vector<float>* outImageData) const;
};

} // namespace rfx
