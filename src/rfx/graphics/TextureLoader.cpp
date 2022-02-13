#include "rfx/pch.h"
#include "rfx/graphics/TextureLoader.h"
#include "rfx/graphics/ImageLoader.h"
#include "rfx/common/Math.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

static const string KTX_FILE_EXTENSION = ".ktx";
static const string HDR_FILE_EXTENSION = ".hdr";

// ---------------------------------------------------------------------------------------------------------------------

vec3 faceCoordsToXYZ(int i, int j, int faceID, int faceSize)
{
    const float A = 2.0f * static_cast<float>(i) / static_cast<float>(faceSize);
    const float B = 2.0f * static_cast<float>(j) / static_cast<float>(faceSize);

    if (faceID == 0) return { -1.0f, A - 1.0f, B - 1.0f };
    if (faceID == 1) return { A - 1.0f, -1.0f, 1.0f - B };
    if (faceID == 2) return { 1.0f, A - 1.0f, 1.0f - B };
    if (faceID == 3) return { 1.0f - A, 1.0f, 1.0f - B };
    if (faceID == 4) return { B - 1.0f, A - 1.0f, 1.0f };
    if (faceID == 5) return { 1.0f - B, A - 1.0f, -1.0f };

    return {};
}

// ---------------------------------------------------------------------------------------------------------------------

vec4 getPixelFloat(
    int x,
    int y,
    const ImageDesc& imageDesc,
    const vector<std::byte>& imageData)
{
    const uint32_t offset = imageDesc.channels * (y * imageDesc.width + x);
    const auto data = reinterpret_cast<const float*>(imageData.data());

    return {
        imageDesc.channels > 0 ? data[offset + 0] : 0.0f,
        imageDesc.channels > 1 ? data[offset + 1] : 0.0f,
        imageDesc.channels > 2 ? data[offset + 2] : 0.0f,
        imageDesc.channels > 3 ? data[offset + 3] : 0.0f
    };
}

// ---------------------------------------------------------------------------------------------------------------------

void setPixelFloat(
    int x,
    int y,
    const vec4& c,
    const ImageDesc& imageDesc,
    vector<std::byte>* outImageData)
{
    const uint32_t offset = imageDesc.channels * (y * imageDesc.width + x);
    const auto data = reinterpret_cast<float*>(outImageData->data());

    if (imageDesc.channels > 0) data[offset + 0] = c.x;
    if (imageDesc.channels > 1) data[offset + 1] = c.y;
    if (imageDesc.channels > 2) data[offset + 2] = c.z;
    if (imageDesc.channels > 3) data[offset + 3] = c.w;
}

// ---------------------------------------------------------------------------------------------------------------------

TextureLoader::TextureLoader(GraphicsDevicePtr graphicsDevice)
    : graphicsDevice(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

Texture2DPtr TextureLoader::loadTexture2D(const path& filePath) const
{
    ImageDesc imageDesc {};
    vector<std::byte> imageData;
    bool createMipmaps = false;


    loadImage(
        filePath,
        ImageChannelType::UNSIGNED_BYTE,
        imageDesc,
        imageData,
        createMipmaps);

    return graphicsDevice->createTexture2D(
        filePath.filename().string(),
        imageDesc,
        imageData,
        createMipmaps);
}

// ---------------------------------------------------------------------------------------------------------------------

void TextureLoader::loadImage(
    const path& filePath,
    ImageChannelType channelType,
    ImageDesc& outImageDesc,
    vector<std::byte>& outImageData,
    bool& outCreateMipmaps) const
{
    const path absoluteImagePath =
        filePath.is_absolute() ? filePath : current_path() / filePath;
    const string extension = filePath.extension().string();

    RFX_CHECK_STATE(exists(absoluteImagePath), "File not found: " + absoluteImagePath.string());

    if (extension == KTX_FILE_EXTENSION) {
        loadFromKTXFile(
            absoluteImagePath,
            outImageDesc,
            outImageData);
        outCreateMipmaps = false;
    }
    else {
        loadFromImageFile(
            absoluteImagePath,
            channelType,
            outImageDesc,
            outImageData);
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


    const auto channels = static_cast<uint32_t>(header.gl_format == GL_RGBA ? 4 : 3);
    const auto layers = static_cast<uint32_t>(texture->isCubemap ? 6 : 1);

    outImageDesc = {
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .width = texture->baseWidth,
        .height = texture->baseHeight,
        .bytesPerPixel = bytesPerPixel,
        .channels = channels,
        .layers = layers,
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
    ImageChannelType channelType,
    ImageDesc& outImageDesc,
    vector<std::byte>& outImageData) const
{
    ImageLoader imageLoader;

    imageLoader.load(
        path,
        channelType,
        4, // desired channels
        &outImageDesc,
        &outImageData);
}

// ---------------------------------------------------------------------------------------------------------------------

CubeMapPtr TextureLoader::loadCubeMap(const path& filePath) const
{
    ImageDesc imageDesc {};
    vector<std::byte> imageData;
    bool createMipmaps = false;

    const string extension = filePath.extension().string();
    const ImageChannelType imageChannelType =
        extension == HDR_FILE_EXTENSION
            ? ImageChannelType::FLOAT
            : ImageChannelType::UNSIGNED_BYTE;

    loadImage(
        filePath,
        imageChannelType,
        imageDesc,
        imageData,
        createMipmaps);

    if (extension == HDR_FILE_EXTENSION) {
        ImageDesc verticalCrossImageDesc {};
        vector<std::byte> verticalCrossImageData;
        convertEquiRectangularMapToVerticalCross(
            imageDesc,
            imageData,
            &verticalCrossImageDesc,
            &verticalCrossImageData);

        convertVerticalCrossToCubeMapFaces(
            verticalCrossImageDesc,
            verticalCrossImageData,
            &imageDesc,
            &imageData);

        createMipmaps = true;
    }


    return graphicsDevice->createCubeMap(
        filePath.filename().string(),
        imageDesc,
        imageData,
        createMipmaps);
}

// ---------------------------------------------------------------------------------------------------------------------

void TextureLoader::convertEquiRectangularMapToVerticalCross(
    const ImageDesc& inImageDesc,
    const vector<std::byte>& inImageData,
    ImageDesc* outImageDesc,
    vector<std::byte>* outImageData) const
{
    const int faceSize = inImageDesc.width / 4;

    const int width = faceSize * 3;
    const int height = faceSize * 4;

    *outImageDesc = inImageDesc;
    outImageDesc->width = width;
    outImageDesc->height = height;

    const size_t outImageDataSize = width * height * 4 * sizeof(float);
    outImageData->resize(outImageDataSize);

    const ivec2 kFaceOffsets[] =
        {
            ivec2(faceSize, faceSize * 3),
            ivec2(0, faceSize),
            ivec2(faceSize, faceSize),
            ivec2(faceSize * 2, faceSize),
            ivec2(faceSize, 0),
            ivec2(faceSize, faceSize * 2)
        };

    const int clampW = static_cast<int>(inImageDesc.width - 1);
    const int clampH = static_cast<int>(inImageDesc.height - 1);

    for (int face = 0; face < 6; ++face)
    {
        for (int i = 0; i < faceSize; ++i)
        {
            for (int j = 0; j < faceSize; ++j)
            {
                const vec3 P = faceCoordsToXYZ(i, j, face, faceSize);
                const float R = hypot(P.x, P.y);
                const float theta = atan2(P.y, P.x);
                const float phi = atan2(P.z, R);
                //	float point source coordinates
                const float Uf = 2.0f * static_cast<float>(faceSize) * (theta + Math::PI) / Math::PI;
                const float Vf = 2.0f * static_cast<float>(faceSize) * (Math::PI / 2.0f - phi) / Math::PI;
                // 4-samples for bilinear interpolation
                const int U1 = glm::clamp(int(floor(Uf)), 0, clampW);
                const int V1 = glm::clamp(int(floor(Vf)), 0, clampH);
                const int U2 = glm::clamp(U1 + 1, 0, clampW);
                const int V2 = glm::clamp(V1 + 1, 0, clampH);
                // fractional part
                const float s = Uf - static_cast<float>(U1);
                const float t = Vf - static_cast<float>(V1);
                // fetch 4-samples
                const vec4 A = getPixelFloat(U1, V1, inImageDesc, inImageData);
                const vec4 B = getPixelFloat(U2, V1, inImageDesc, inImageData);
                const vec4 C = getPixelFloat(U1, V2, inImageDesc, inImageData);
                const vec4 D = getPixelFloat(U2, V2, inImageDesc, inImageData);
                // bilinear interpolation
                const vec4 color = A * (1 - s) * (1 - t) + B * (s) * (1 - t) + C * (1 - s) * t + D * (s) * (t);
                setPixelFloat(
                    i + kFaceOffsets[face].x,
                    j + kFaceOffsets[face].y,
                    color,
                    *outImageDesc,
                    outImageData);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TextureLoader::convertVerticalCrossToCubeMapFaces(
    const ImageDesc& inImageDesc,
    const std::vector<std::byte>& inImageData,
    ImageDesc* outImageDesc,
    vector<std::byte>* outImageData) const
{
    const int faceWidth = inImageDesc.width / 3;
    const int faceHeight = inImageDesc.height / 4;

    *outImageDesc = inImageDesc;
    outImageDesc->width = faceWidth;
    outImageDesc->height = faceHeight;
    outImageDesc->layers = 6;
    outImageDesc->isCubemap = true;

    outImageData->resize(faceWidth * faceHeight * 6 * inImageDesc.bytesPerPixel);


    auto src = reinterpret_cast<const uint8_t*>(inImageData.data());
    auto dst = reinterpret_cast<uint8_t*>(outImageData->data());

    /*
            ------
            | +Y |
       ----------------
       | -X | -Z | +X |
       ----------------
            | -Y |
            ------
            | +Z |
            ------
    */

    const uint32_t pixelSize = inImageDesc.bytesPerPixel;

    for (int face = 0; face < 6; ++face)
    {
        for (int j = 0; j < faceHeight; ++j)
        {
            for (int i = 0; i < faceWidth; ++i)
            {
                int x = 0;
                int y = 0;

                switch (face)
                {
                    // GL_TEXTURE_CUBE_MAP_POSITIVE_X
                    case 0:
                        x = i;
                        y = faceHeight + j;
                        break;

                        // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
                    case 1:
                        x = 2 * faceWidth + i;
                        y = 1 * faceHeight + j;
                        break;

                        // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
                    case 2:
                        x = 2 * faceWidth - (i + 1);
                        y = 1 * faceHeight - (j + 1);
                        break;

                        // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
                    case 3:
                        x = 2 * faceWidth - (i + 1);
                        y = 3 * faceHeight - (j + 1);
                        break;

                        // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
                    case 4:
                        x = 2 * faceWidth - (i + 1);
                        y = inImageDesc.height - (j + 1);
                        break;

                        // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
                    case 5:
                        x = faceWidth + i;
                        y = faceHeight + j;
                        break;
                }

                memcpy(dst, src + (y * inImageDesc.width + x) * pixelSize, pixelSize);

                dst += pixelSize;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
