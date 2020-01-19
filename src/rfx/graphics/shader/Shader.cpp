#include "rfx/pch.h"
#include "rfx/graphics/shader/Shader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Shader::Shader(
    VkDevice vkDevice,
    const VulkanDeviceFunctionPtrs& vk,
    const VkPipelineShaderStageCreateInfo& stageCreateInfo)
        : vkDevice(vkDevice),
          vkDestroyShaderModule(vk.vkDestroyShaderModule),
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
        stageCreateInfo.module = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const VkPipelineShaderStageCreateInfo& Shader::getStageCreateInfo() const
{
    return stageCreateInfo;
}

// ---------------------------------------------------------------------------------------------------------------------
