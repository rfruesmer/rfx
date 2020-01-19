#include "rfx/pch.h"
#include "rfx/graphics/Shader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Shader::Shader(const std::shared_ptr<GraphicsDevice>& graphicsDevice, 
    const VkPipelineShaderStageCreateInfo& stageCreateInfo)
        : graphicsDevice(graphicsDevice),
          stageCreateInfo(stageCreateInfo) {}

// ---------------------------------------------------------------------------------------------------------------------

Shader::~Shader()
{
    dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void Shader::dispose()
{
    if (stageCreateInfo.module) {
        graphicsDevice->destroyShaderModule(stageCreateInfo.module);
        stageCreateInfo.module = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const VkPipelineShaderStageCreateInfo& Shader::getStageCreateInfo() const
{
    return stageCreateInfo;
}

// ---------------------------------------------------------------------------------------------------------------------
