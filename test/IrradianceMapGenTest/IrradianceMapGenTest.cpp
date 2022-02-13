#include "rfx/pch.h"
#include "IrradianceMapGenTest.h"
#include "rfx/common/Logger.h"
#include "rfx/graphics/ImageLoader.h"

#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define  STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#include "rfx/common/Math.h"


using namespace rfx;
using namespace rfx::test;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    try {
        RFX_CHECK_ARGUMENT(argc >= 3);

        auto theApp = make_shared<IrradianceMapGenTest>();
        theApp->run(argv[1], argv[2]);
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::run(const path& inputPath, const path& outputPath)
{
    initialize();
    convert(inputPath, outputPath);
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::initialize()
{
    initLogging();
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::initLogging()
{
#ifdef _DEBUG
    Logger::setLogLevel(LogLevel::DEBUG);
#endif // _DEBUG
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::convert(
    const path& sourceImagePath,
    const path& destImagePath)
{
    const int destImageWidth = 256;
    const int destImageHeight = 128;
    const int numPoints = 1024;

    ImageDesc sourceImageDesc {};
    vector<float> sourceImageData;
    loadSourceImage(
        sourceImagePath,
        sourceImageDesc,
        sourceImageData);

    vector<vec3> destImageData(destImageWidth * destImageHeight);

    convolveDiffuse(
        sourceImageData.data(),
        sourceImageDesc,
        destImageWidth,
        destImageHeight,
        numPoints,
        destImageData.data());

    writeDestImage(
        destImagePath,
        destImageWidth,
        destImageHeight,
        destImageData);
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::loadSourceImage(
    const path& imageFilePath,
    ImageDesc& outImageDesc,
    vector<float>& outImageData) const
{
    ImageLoader imageLoader;
    imageLoader.load(
        imageFilePath,
        3,
        &outImageDesc,
        &outImageData);
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::convolveDiffuse(
    float* sourceImageData,
    const ImageDesc& sourceImageDesc,
    size_t destImageWidth,
    size_t destImageHeight,
    size_t monteCarloSampleCount,
    glm::vec3* destImageData)
{
    RFX_CHECK_ARGUMENT(sourceImageDesc.width == 2 * sourceImageDesc.height);

    vector<vec3> tmp(destImageWidth * destImageHeight);

    stbir_resize_float_generic(
        reinterpret_cast<const float*>(sourceImageData),
        static_cast<int>(sourceImageDesc.width),
        static_cast<int>(sourceImageDesc.height),
        0,
        reinterpret_cast<float*>(tmp.data()),
        static_cast<int>(destImageWidth),
        static_cast<int>(destImageHeight),
        0,
        3,
        STBIR_ALPHA_CHANNEL_NONE,
        0,
        STBIR_EDGE_CLAMP,
        STBIR_FILTER_CUBICBSPLINE,
        STBIR_COLORSPACE_LINEAR,
        nullptr);

    const vec3* scratch = tmp.data();

    for (int y = 0; y != destImageHeight; y++)
    {
        const float theta1 = float(y) / float(destImageHeight) * Math::PI;
        for (int x = 0; x != destImageWidth; x++)
        {
            const float phi1 = float(x) / float(destImageWidth) * Math::TWO_PI;
            const vec3 V1 = vec3(sin(theta1) * cos(phi1), sin(theta1) * sin(phi1), cos(theta1));
            vec3 color = vec3(0.0f);
            float weight = 0.0f;
            for (int i = 0; i != monteCarloSampleCount; i++)
            {
                const vec2 h = hammersley2d(i, monteCarloSampleCount);
                const int x1 = int(floor(h.x * float(destImageWidth)));
                const int y1 = int(floor(h.y * float(destImageHeight)));
                const float theta2 = float(y1) / float(destImageHeight) * Math::PI;
                const float phi2 = float(x1) / float(destImageWidth) * Math::TWO_PI;
                const vec3 V2 = vec3(sin(theta2) * cos(phi2), sin(theta2) * sin(phi2), cos(theta2));
                const float D = std::max(0.0f, dot(V1, V2));
                if (D > 0.01f)
                {
                    color += scratch[y1 * destImageWidth + x1] * D;
                    weight += D;
                }
            }

            destImageData[y * destImageWidth + x] = color / weight;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

/// From http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
vec2 IrradianceMapGenTest::hammersley2d(uint32_t i, uint32_t N)
{
    return vec2(float(i) / float(N), radicalInverse_VdC(i));
}

// ---------------------------------------------------------------------------------------------------------------------

/// From Henry J. Warren's "Hacker's Delight"
float IrradianceMapGenTest::radicalInverse_VdC(uint32_t bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
}

// ---------------------------------------------------------------------------------------------------------------------

void IrradianceMapGenTest::writeDestImage(
    const path& destImagePath,
    int destImageWidth,
    int destImageHeight,
    vector<vec3>& destImageData) const
{
    stbi_write_hdr(
        destImagePath.string().c_str(),
        destImageWidth,
        destImageHeight,
        3,
        reinterpret_cast<float*>(destImageData.data()));
}

// ---------------------------------------------------------------------------------------------------------------------
