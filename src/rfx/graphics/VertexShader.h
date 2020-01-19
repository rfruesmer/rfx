#pragma once

#include "rfx/graphics/Shader.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

class VertexShader : public Shader
{
public:
    explicit VertexShader(const std::shared_ptr<GraphicsDevice>& graphicsDevice, 
        const VkPipelineShaderStageCreateInfo& stageCreateInfo, 
        const VertexFormat& vertexFormat);

    const VertexFormat& getVertexFormat() const;
    const VkPipelineVertexInputStateCreateInfo& getVertexInputState() const;

private:
    void createVertexInputState();

    VertexFormat vertexFormat;
    VkVertexInputBindingDescription vertexBindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
};
    
}
