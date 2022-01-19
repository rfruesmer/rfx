#include "rfx/pch.h"
#include "ComputeShader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

ComputeShader::ComputeShader(
    VkDevice vkDevice,
    const VkPipelineShaderStageCreateInfo& stageCreateInfo)
        : Shader(vkDevice, stageCreateInfo) {}

// ---------------------------------------------------------------------------------------------------------------------

