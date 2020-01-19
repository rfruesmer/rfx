#pragma once

#include "rfx/graphics/shader/Shader.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

class VertexShader : public Shader
{
public:
    explicit VertexShader(
        VkDevice vkDevice,
        const VulkanDeviceFunctionPtrs& vk,
        const VkPipelineShaderStageCreateInfo& createInfo, 
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
