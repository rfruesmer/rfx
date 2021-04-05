#include "rfx/pch.h"
#include "rfx/application/TextureLoader.h"
#include "ImageLoader.h"


using namespace rfx;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

static const string KTX_FILE_EXTENSION = ".ktx";

// ---------------------------------------------------------------------------------------------------------------------

TextureLoader::TextureLoader(GraphicsDevicePtr graphicsDevice)
    : graphicsDevice(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

Texture2DPtr TextureLoader::loadTexture2D(const path& filePath) const
{
    ImageDesc imageDesc {};
    vector<std::byte> imageData;
    bool createMipmaps = false;


    loadImage(filePath, imageDesc, imageData, createMipmaps);

    return graphicsDevice->createTexture2D(
        filePath.filename().string(),
        imageDesc,
        imageData,
        createMipmaps);
}

// ---------------------------------------------------------------------------------------------------------------------

void TextureLoader::loadImage(
    const path& filePath,
    ImageDesc& outImageDesc,
    vector<std::byte>& outImageData,
    bool& outCreateMipmaps) const
{
    const path absoluteImagePath =
        filePath.is_absolute() ? filePath : current_path() / filePath;
    const string extension = filePath.extension().string();

    RFX_CHECK_STATE(exists(absoluteImagePath), "File not found: " + absoluteImagePath.string());

    if (extension == KTX_FILE_EXTENSION) {
        loadFromKTXFile(absoluteImagePath, outImageDesc, outImageData);
        outCreateMipmaps = false;
    }
    else {
        loadFromImageFile(absoluteImagePath, outImageDesc, outImageData);
        outCreateMipmaps = true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TextureLoader::loadFromKTXFile(
    const path& path,
    ImageDesc& outImageDesc,
    vector<std::byte>& outImageData) const
{
    KTXHeader header = readKTXHeader(path);

    uint32_t bytesPerPixel;
    if (header.gl_format == GL_RGBA) {
        bytesPerPixel = 4;
    }
    else if (header.gl_format == GL_RGB) {
        bytesPerPixel = 3;
    }
    else {
        RFX_THROW("Unsupported texture format");
    }

    ktxTexture* texture = nullptr;
    KTX_error_code result = ktxTexture_CreateFromNamedFile(
        path.string().c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &texture);
    RFX_CHECK_STATE(result == KTX_SUCCESS,
        "Failed to load KTX file: " + path.string());


    outImageDesc = {
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .width = texture->baseWidth,
        .height = texture->baseHeight,
        .bytesPerPixel = bytesPerPixel,
        .mipLevels = texture->numLevels,
        .mipOffsets = {},
        .isCubemap = texture->isCubemap
    };


    for (uint32_t faceIndex = 0, faceCount = texture->isCubemap ? 6 : 1;
         faceIndex < faceCount;
         ++faceIndex)
    {
        for (uint32_t mipLevel = 0; mipLevel < texture->numLevels; ++mipLevel)
        {
            ktx_size_t offset;
            result = ktxTexture_GetImageOffset(texture, mipLevel, 0, faceIndex, &offset);
            RFX_CHECK_STATE(result == KTX_SUCCESS, "");
            outImageDesc.mipOffsets.push_back(offset);
        }
    }

    const ktx_uint8_t* imageData = ktxTexture_GetData(texture);
    const ktx_size_t imageDataSize = ktxTexture_GetDataSize(texture);
    outImageData.resize(imageDataSize);
    memcpy(&outImageData[0], imageData, imageDataSize);

    ktxTexture_Destroy(texture);
}

// ---------------------------------------------------------------------------------------------------------------------

TextureLoader::KTXHeader TextureLoader::readKTXHeader(const path& path) const
{
    static const uint8_t expectedIdentifier[] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

    ifstream inputStream(path.string(), ifstream::binary);

    uint8_t identifier[12];
    inputStream.read(reinterpret_cast<char*>(&identifier), sizeof(identifier));
    RFX_CHECK_STATE(memcmp(identifier, expectedIdentifier, sizeof(expectedIdentifier)) == 0, "Invalid KTX file");

    KTXHeader header {};
    inputStream.read(reinterpret_cast<char*>(&header), sizeof(header));

    return header;
}

// ---------------------------------------------------------------------------------------------------------------------

void TextureLoader::loadFromImageFile(
    const path& path,
    ImageDesc& outImageDesc,
    vector<std::byte>& outImageData) const
{
    ImageLoader imageLoader;
    imageLoader.load(path, &outImageDesc, &outImageData);
}

// ---------------------------------------------------------------------------------------------------------------------

CubeMapPtr TextureLoader::loadCubeMap(const path& filePath) const
{
    ImageDesc imageDesc {};
    vector<std::byte> imageData;
    bool createMipmaps = false;


    loadImage(filePath, imageDesc, imageData, createMipmaps);

    return graphicsDevice->createCubeMap(
        filePath.filename().string(),
        imageDesc,
        imageData,
        createMipmaps);
}

// ---------------------------------------------------------------------------------------------------------------------
