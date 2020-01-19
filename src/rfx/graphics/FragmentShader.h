#pragma once

#include "rfx/graphics/Shader.h"

namespace rfx
{

class FragmentShader : public Shader
{
public:
    explicit FragmentShader(const std::shared_ptr<GraphicsDevice>& graphicsDevice, 
        const VkPipelineShaderStageCreateInfo& stageCreateInfo);
};

}
