#pragma once

#include "rfx/graphics/shader/Shader.h"

namespace rfx
{

class FragmentShader : public Shader
{
public:
    explicit FragmentShader(
        VkDevice vkDevice,
        const VulkanDeviceFunctionPtrs& vk,
        const VkPipelineShaderStageCreateInfo& createInfo);
};

}
