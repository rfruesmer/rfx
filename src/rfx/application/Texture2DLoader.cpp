#include "rfx/pch.h"
#include "rfx/application/Texture2DLoader.h"

// STB Header Files
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "ktxvulkan.h"

using namespace rfx;
using namespace std;
using namespace filesystem;


// ---------------------------------------------------------------------------------------------------------------------

static const string KTX_FILE_EXTENSION = ".ktx";

// ---------------------------------------------------------------------------------------------------------------------

Texture2DLoader::Texture2DLoader(const shared_ptr<GraphicsDevice>& graphicsDevice)
    : graphicsDevice(graphicsDevice) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Texture2D> Texture2DLoader::load(const path& imagePath) const
{
    const path absoluteImagePath =
        imagePath.is_absolute() ? imagePath : current_path() / imagePath;
    const string extension = imagePath.extension().string();

    ImageInfo imageInfo = {};
    if (extension == KTX_FILE_EXTENSION) {
        loadFromKTXFile(absoluteImagePath, imageInfo);
    }
    else {
        loadFromImageFile(absoluteImagePath, imageInfo);
    }

    return graphicsDevice->createTexture2D(
        imageInfo.width, 
        imageInfo.height, 
        imageInfo.format,
        imageInfo.data);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DLoader::loadFromKTXFile(const path& path, ImageInfo& outImageInfo) const
{
    ktxTexture* texture = nullptr;
    KTX_error_code result = ktxTexture_CreateFromNamedFile(
        path.string().c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &texture);
    RFX_CHECK_STATE(result == KTX_SUCCESS, 
        "Failed to load KTX file: " + path.string());

    const ktx_uint32_t level = 0;
    const ktx_uint32_t layer = 0;
    const ktx_uint32_t faceSlice = 0;
    ktx_size_t offset = 0;

    result = ktxTexture_GetImageOffset(texture, level, layer, faceSlice, &offset);
    RFX_CHECK_STATE(result == KTX_SUCCESS, 
        "Failed to get image offset for: " + path.string());

    const ktx_uint8_t* imageData = ktxTexture_GetData(texture) + offset;
    const ktx_size_t imageDataSize = ktxTexture_GetSize(texture);

    outImageInfo.width = texture->baseWidth;
    outImageInfo.height = texture->baseHeight;
    outImageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    outImageInfo.data.resize(imageDataSize);
    memcpy(&outImageInfo.data[0], imageData, imageDataSize);

    ktxTexture_Destroy(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DLoader::loadFromImageFile(const path& path, ImageInfo& outImageInfo) const
{
    int bytesPerPixel = 0;

    outImageInfo.mipMapLevels = 1;

    void* imageData = stbi_load(path.string().c_str(), 
        &outImageInfo.width, 
        &outImageInfo.height, 
        &bytesPerPixel, 
        STBI_rgb_alpha);

    RFX_CHECK_STATE(imageData != nullptr 
            && outImageInfo.width > 0
            && outImageInfo.height > 0
            && bytesPerPixel == 4, 
            "Failed to load image");

    const size_t imageDataSize = outImageInfo.width * outImageInfo.height * STBI_rgb_alpha;
    outImageInfo.data.resize(imageDataSize);
    memcpy(&outImageInfo.data[0], imageData, imageDataSize);

    stbi_image_free(imageData);
}

// ---------------------------------------------------------------------------------------------------------------------
