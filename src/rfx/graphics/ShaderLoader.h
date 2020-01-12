#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class ShaderLoader
{
public:
    explicit ShaderLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    VkPipelineShaderStageCreateInfo load(
        const std::filesystem::path& path, 
        VkShaderStageFlagBits stage,
        const char* entryPoint);

private:
    std::shared_ptr<GraphicsDevice> graphicsDevice;
};
    
}
