#pragma once

#include "rfx/graphics/Shader.h"


namespace rfx {

class ComputeShader : public Shader
{
public:
    ComputeShader(
        VkDevice vkDevice,
        const VkPipelineShaderStageCreateInfo& stageCreateInfo);
};

using ComputeShaderPtr = std::shared_ptr<ComputeShader>;

} // namespace rfx
