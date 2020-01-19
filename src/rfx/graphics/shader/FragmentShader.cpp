#include "rfx/pch.h"
#include "rfx/graphics/shader/FragmentShader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

FragmentShader::FragmentShader(
    VkDevice vkDevice,
    const VulkanDeviceFunctionPtrs& vk,
    const VkPipelineShaderStageCreateInfo& createInfo)
        : Shader(vkDevice, vk, createInfo) {}

// ---------------------------------------------------------------------------------------------------------------------
