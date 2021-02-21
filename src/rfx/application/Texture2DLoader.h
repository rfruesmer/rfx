#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/Texture2D.h"
#include "rfx/graphics/ImageDesc.h"


namespace rfx {

class Texture2DLoader
{
public:
    explicit Texture2DLoader(std::shared_ptr<GraphicsDevice> graphicsDevice);

    [[nodiscard]]
    std::shared_ptr<Texture2D> load(const std::filesystem::path& filePath) const;

private:
#pragma pack(push, 4)
    struct KTXHeader {
        uint32_t endianness = 0;
        uint32_t gl_type = 0;
        uint32_t gl_type_size = 0;
        uint32_t gl_format = 0;
        uint32_t gl_internal_format = 0;
        uint32_t gl_base_internal_format = 0;
        uint32_t pixel_width = 0;
        uint32_t pixel_height = 0;
        uint32_t pixel_depth = 0;
        uint32_t number_of_array_elements = 0;
        uint32_t number_of_faces = 0;
        uint32_t number_of_mipmap_levels = 0;
        uint32_t bytes_of_key_value_data = 0;
    };
#pragma pack(pop)

    void loadFromKTXFile(
        const std::filesystem::path& path,
        ImageDesc& outImageDesc,
        std::vector<std::byte>& outImageData) const;

    KTXHeader readKTXHeader(const std::filesystem::path& path) const;

    void loadFromImageFile(
        const std::filesystem::path& path,
        ImageDesc& outImageDesc,
        std::vector<std::byte>& outImageData) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx;