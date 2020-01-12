#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class Texture2DLoader
{
public:
    explicit Texture2DLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    std::unique_ptr<Texture2D> load(const std::filesystem::path& imagePath);

private:
    struct ImageInfo
    {
        int width;
        int height;
        int bytesPerPixel;
        int mipMapLevels;
        std::vector<std::byte> data;
    };

    void loadFromDDSFile(const std::filesystem::path& path);
    void loadFromImageFile(const std::filesystem::path& path, ImageInfo& outImageInfo);

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx
