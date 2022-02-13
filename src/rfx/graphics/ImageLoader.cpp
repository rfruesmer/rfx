#include "rfx/pch.h"
#include "rfx/graphics/ImageLoader.h"

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
    load(
        imagePath,
        ImageChannelType::UNSIGNED_BYTE,
        STBI_rgb_alpha,
        outImageDesc,
        outImageData);
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageLoader::load(
    const path& imagePath,
    int desiredChannels,
    ImageDesc* outImageDesc,
    vector<std::byte>* outImageData) const
{
    load(
        imagePath,
        ImageChannelType::UNSIGNED_BYTE,
        desiredChannels,
        outImageDesc,
        outImageData);
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageLoader::load(
    const path& imagePath,
    ImageChannelType channelType,
    int desiredChannels,
    ImageDesc* outImageDesc,
    vector<std::byte>* outImageData) const
{
    RFX_CHECK_ARGUMENT(desiredChannels == 3 || desiredChannels == 4);

    int channelsInFile = 0;
    void* imageData;

    if (channelType == ImageChannelType::UNSIGNED_BYTE) {
        imageData = stbi_load(
            imagePath.string().c_str(),
            reinterpret_cast<int*>(&outImageDesc->width),
            reinterpret_cast<int*>(&outImageDesc->height),
            &channelsInFile,
            desiredChannels);
    }
    else {
        imageData = stbi_loadf(
            imagePath.string().c_str(),
            reinterpret_cast<int*>(&outImageDesc->width),
            reinterpret_cast<int*>(&outImageDesc->height),
            &channelsInFile,
            desiredChannels);
    }

    RFX_CHECK_STATE(imageData != nullptr && outImageDesc->width > 0 && outImageDesc->height > 0, "Failed to load image");

    if (channelType == ImageChannelType::UNSIGNED_BYTE) {
        outImageDesc->bytesPerPixel = desiredChannels * sizeof(std::byte);
        outImageDesc->format = desiredChannels == 4
               ? VK_FORMAT_R8G8B8A8_SRGB
               : VK_FORMAT_R8G8B8_SRGB;
    }
    else {
        outImageDesc->bytesPerPixel = desiredChannels * sizeof(float);
        outImageDesc->format = desiredChannels == 4
               ? VK_FORMAT_R32G32B32A32_SFLOAT
               : VK_FORMAT_R32G32B32_SFLOAT;
    }
    outImageDesc->channels = desiredChannels;
    outImageDesc->mipLevels = 1;
    outImageDesc->mipOffsets = { 0 };

    const size_t byteCount = outImageDesc->width * outImageDesc->height * outImageDesc->bytesPerPixel;
    outImageData->resize(byteCount);
    memcpy(outImageData->data(), imageData, byteCount);

    stbi_image_free(imageData);
}

// ---------------------------------------------------------------------------------------------------------------------

void ImageLoader::load(
    const path& imagePath,
    int desiredChannels,
    ImageDesc* outImageDesc,
    vector<float>* outImageData) const
{
    load(
        imagePath,
        ImageChannelType::FLOAT,
        desiredChannels,
        outImageDesc,
        reinterpret_cast<vector<std::byte>*>(outImageData));
}

// ---------------------------------------------------------------------------------------------------------------------
