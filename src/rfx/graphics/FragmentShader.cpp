#include "rfx/pch.h"
#include "rfx/graphics/FragmentShader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

FragmentShader::FragmentShader(
    VkDevice vkDevice,
    const VkPipelineShaderStageCreateInfo& createInfo)
        : Shader(vkDevice, createInfo) {}

// ---------------------------------------------------------------------------------------------------------------------
