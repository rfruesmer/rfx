#include "rfx/pch.h"
#include "rfx/graphics/Shader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Shader::Shader(
    VkDevice vkDevice,
    const VkPipelineShaderStageCreateInfo& stageCreateInfo)
        : vkDevice(vkDevice),
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
        vkDestroyShaderModule(vkDevice, stageCreateInfo.module, nullptr);
        stageCreateInfo.module = VK_NULL_HANDLE;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const VkPipelineShaderStageCreateInfo& Shader::getStageCreateInfo() const
{
    return stageCreateInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

VkShaderModule Shader::getModule() const
{
    return stageCreateInfo.module;
}

// ---------------------------------------------------------------------------------------------------------------------
