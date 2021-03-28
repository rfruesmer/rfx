#pragma once

#include "rfx/graphics/Shader.h"

namespace rfx {

class FragmentShader : public Shader
{
public:
    FragmentShader(
        VkDevice vkDevice,
        const VkPipelineShaderStageCreateInfo& createInfo);
};

using FragmentShaderPtr = std::shared_ptr<FragmentShader>;

} // namespace rfx
