#pragma once


#include "rfx/graphics/ImageDesc.h"

namespace rfx::test {

class IrradianceMapGenTest
{
public:
    void run(
        const std::filesystem::path& inputPath,
        const std::filesystem::path& outputPath);

private:
    void initialize();
    static void initLogging();

    void convert(
        const std::filesystem::path& sourceImagePath,
        const std::filesystem::path& destImagePath);

    void loadSourceImage(
        const std::filesystem::path& imageFilePath,
        ImageDesc& outImageDesc,
        std::vector<float>& outImageData) const;

    void convolveDiffuse(
        float* sourceImageData,
        const ImageDesc& sourceImageDesc,
        size_t destImageWidth,
        size_t destImageHeight,
        size_t monteCarloSampleCount,
        glm::vec3* destImageData);
    static glm::vec2 hammersley2d(uint32_t i, uint32_t N);
    static float radicalInverse_VdC(uint32_t bits);

    void writeDestImage(
        const std::filesystem::path& destImagePath,
        int destImageWidth,
        int destImageHeight,
        std::vector<glm::vec3>& destImageData) const;
};

} // namespace rfx::test