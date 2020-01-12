#include "rfx/pch.h"
#include "rfx/graphics/Texture2DLoader.h"

// STB Header Files
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture2DLoader::Texture2DLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : graphicsDevice(graphicsDevice) {}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Texture2D> Texture2DLoader::load(const filesystem::path& imagePath)
{
    ImageInfo imageInfo = {};
    const filesystem::path absoluteImagePath =
        imagePath.is_absolute() ? imagePath : filesystem::current_path() / imagePath;
    const string extension = imagePath.extension().string();


    if (extension == ".dds") {
        loadFromDDSFile(absoluteImagePath);
    }
    else {
        loadFromImageFile(absoluteImagePath, imageInfo);
    }

    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    switch (imageInfo.bytesPerPixel) {
    case 3:
        imageFormat = VK_FORMAT_R8G8B8_UNORM;
        break;
    case 4:
        imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        break;
    default:
        RFX_THROW("Invalid image format, bytesPerPixel=" + to_string(imageInfo.bytesPerPixel));
    }

    return graphicsDevice->createTexture2D(
        imageInfo.width, 
        imageInfo.height, 
        imageInfo.bytesPerPixel,
        imageFormat,        
        imageInfo.data);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DLoader::loadFromDDSFile(const filesystem::path& path)
{
    RFX_THROW(".dds file support not implemented yet");   
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DLoader::loadFromImageFile(const std::filesystem::path& path, ImageInfo& outImageInfo)
{
    outImageInfo.mipMapLevels = 1;

    void* imageData = stbi_load(path.string().c_str(), 
        &outImageInfo.width, 
        &outImageInfo.height, 
        &outImageInfo.bytesPerPixel, 
        STBI_rgb_alpha);

    RFX_CHECK_STATE(imageData != nullptr 
            && outImageInfo.width > 0
            && outImageInfo.height > 0
            && outImageInfo.bytesPerPixel > 0, 
            "Failed to load image");

    const size_t imageDataSize = outImageInfo.width * outImageInfo.height * STBI_rgb_alpha;
    outImageInfo.data.resize(imageDataSize);
    std::memcpy(&outImageInfo.data[0], imageData, imageDataSize);

    stbi_image_free(imageData);
}

// ---------------------------------------------------------------------------------------------------------------------
