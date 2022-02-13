#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/Texture2D.h"
#include "rfx/graphics/CubeMap.h"
#include "rfx/graphics/ImageDesc.h"
#include "rfx/graphics/ImageChannelType.h"


namespace rfx {

class TextureLoader
{
public:
    explicit TextureLoader(GraphicsDevicePtr graphicsDevice);

    [[nodiscard]]
    Texture2DPtr loadTexture2D(const std::filesystem::path& filePath) const;

    [[nodiscard]]
    CubeMapPtr loadCubeMap(const std::filesystem::path& filePath) const;

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

    void loadImage(
        const std::filesystem::path& filePath,
        ImageChannelType channelType,
        ImageDesc& outImageDesc,
        std::vector<std::byte>& outImageData,
        bool& outCreateMipmaps) const;

    void loadFromKTXFile(
        const std::filesystem::path& path,
        ImageDesc& outImageDesc,
        std::vector<std::byte>& outImageData) const;

    KTXHeader readKTXHeader(const std::filesystem::path& path) const;

    void loadFromImageFile(
        const std::filesystem::path& path,
        ImageChannelType channelType,
        ImageDesc& outImageDesc,
        std::vector<std::byte>& outImageData) const;

    void convertEquiRectangularMapToVerticalCross(
        const ImageDesc& inImageDesc,
        const std::vector<std::byte>& inImageData,
        ImageDesc* outImageDesc,
        std::vector<std::byte>* outImageData) const;

    void convertVerticalCrossToCubeMapFaces(
        const ImageDesc& inImageDesc,
        const std::vector<std::byte>& inImageData,
        ImageDesc* outImageDesc,
        std::vector<std::byte>* outImageData) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};

} // namespace rfx;