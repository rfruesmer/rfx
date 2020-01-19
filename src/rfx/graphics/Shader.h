#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class Shader
{
public:
    explicit Shader(const std::shared_ptr<GraphicsDevice>& graphicsDevice, 
        const VkPipelineShaderStageCreateInfo& stageCreateInfo);
    virtual ~Shader();

    void dispose();

    const VkPipelineShaderStageCreateInfo& getStageCreateInfo() const;

private:
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    VkPipelineShaderStageCreateInfo stageCreateInfo = {};
};
    
} // namespace rfx
