#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/Texture2D.h"

namespace rfx {

class Texture2DLoader
{
public:
    explicit Texture2DLoader(std::shared_ptr<GraphicsDevice> graphicsDevice);

    [[nodiscard]]
    std::shared_ptr<Texture2D> load(const std::filesystem::path& filePath) const;

private:
    struct ImageDesc
    {
        int width;
        int height;
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        int mipMapLevels;
        std::vector<std::byte> data;
    };

    void loadFromKTXFile(const std::filesystem::path& path, ImageDesc& outImageDesc) const;
    void loadFromImageFile(const std::filesystem::path& path, ImageDesc& outImageDesc) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx;