#include "rfx/pch.h"
#include "rfx/application/Texture2DLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_DEFINE
#include "stb.h"

using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

static const string KTX_FILE_EXTENSION = ".ktx";

// ---------------------------------------------------------------------------------------------------------------------

Texture2DLoader::Texture2DLoader(shared_ptr<GraphicsDevice> graphicsDevice)
    : graphicsDevice(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Texture2D> Texture2DLoader::load(const filesystem::path& filePath) const
{
    const path absoluteImagePath =
        filePath.is_absolute() ? filePath : current_path() / filePath;
    const string extension = filePath.extension().string();

    ImageDesc imageDesc = {};
    if (extension == KTX_FILE_EXTENSION) {
        loadFromKTXFile(absoluteImagePath, imageDesc);
    }
    else {
        loadFromImageFile(absoluteImagePath, imageDesc);
    }

    return graphicsDevice->createTexture2D(
        imageDesc.width,
        imageDesc.height,
        imageDesc.format,
        imageDesc.data);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DLoader::loadFromKTXFile(const path& path, ImageDesc& outImageDesc) const
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
    const ktx_size_t imageDataSize = ktxTexture_GetDataSize(texture);

    outImageDesc.width = texture->baseWidth;
    outImageDesc.height = texture->baseHeight;
    outImageDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
    outImageDesc.data.resize(imageDataSize);
    memcpy(&outImageDesc.data[0], imageData, imageDataSize);

    ktxTexture_Destroy(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DLoader::loadFromImageFile(const path& path, ImageDesc& outImageDesc) const
{
    int bytesPerPixel = 0;

    outImageDesc.mipMapLevels = 1;

    void* imageData = stbi_load(path.string().c_str(),
        &outImageDesc.width,
        &outImageDesc.height,
        &bytesPerPixel,
        STBI_rgb_alpha);

    RFX_CHECK_STATE(imageData != nullptr
                    && outImageDesc.width > 0
                    && outImageDesc.height > 0
                    && bytesPerPixel == 4,
        "Failed to load image");

    const size_t imageDataSize = outImageDesc.width * outImageDesc.height * STBI_rgb_alpha;
    outImageDesc.data.resize(imageDataSize);
    memcpy(&outImageDesc.data[0], imageData, imageDataSize);

    stbi_image_free(imageData);
}

// ---------------------------------------------------------------------------------------------------------------------
