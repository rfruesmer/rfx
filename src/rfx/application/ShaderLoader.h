#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/shader/VertexShader.h"
#include "rfx/graphics/shader/FragmentShader.h"

namespace rfx
{

class ShaderLoader
{
public:
    explicit ShaderLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    std::shared_ptr<VertexShader> loadVertexShader(
        const std::filesystem::path& path,
        const char* entryPoint,
        const VertexFormat& vertexFormat) const;

    std::shared_ptr<FragmentShader> loadFragmentShader(
        const std::filesystem::path& path,
        const char* entryPoint) const;

private:
    VkPipelineShaderStageCreateInfo loadInternal(
        const std::filesystem::path& path,
        VkShaderStageFlagBits stage,
        const char* entryPoint) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};
    
}
