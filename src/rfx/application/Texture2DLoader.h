#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class Texture2DLoader
{
public:
    explicit Texture2DLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    std::shared_ptr<Texture2D> load(const std::filesystem::path& imagePath) const;

private:
    struct ImageInfo
    {
        int width;
        int height;
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        int mipMapLevels;
        std::vector<std::byte> data;
    };

    void loadFromKTXFile(const std::filesystem::path& path, ImageInfo& outImageInfo) const;
    void loadFromImageFile(const std::filesystem::path& path, ImageInfo& outImageInfo) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx
