#include "rfx/pch.h"
#include "rfx/application/ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_DEFINE
#include "stb.h"


using namespace rfx;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

void ImageLoader::load(
    const path& imagePath, 
    ImageDesc* outImageDesc, 
    vector<std::byte>* outImageData) const
{
    int bytesPerPixelInFile = 0;
    void* imageData = stbi_load(imagePath.string().c_str(),
        reinterpret_cast<int*>(&outImageDesc->width),
        reinterpret_cast<int*>(&outImageDesc->height),
        &bytesPerPixelInFile,
        STBI_rgb_alpha);

    RFX_CHECK_STATE(imageData != nullptr && outImageDesc->width > 0 && outImageDesc->height > 0, "Failed to load image");

    outImageDesc->bytesPerPixel = 4;
    outImageDesc->format = VK_FORMAT_R8G8B8A8_SRGB;
    outImageDesc->mipLevels = 1;
    outImageDesc->mipOffsets = { 0 };

    const size_t imageDataSize = outImageDesc->width * outImageDesc->height * outImageDesc->bytesPerPixel;
    outImageData->resize(imageDataSize);
    memcpy(outImageData->data(), imageData, imageDataSize);

    stbi_image_free(imageData);
}

// ---------------------------------------------------------------------------------------------------------------------
