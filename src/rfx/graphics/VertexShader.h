#pragma once

#include "rfx/graphics/Shader.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx {

class VertexShader : public Shader
{
public:
    VertexShader(
        VkDevice vkDevice,
        const VkPipelineShaderStageCreateInfo& createInfo,
        const VertexFormat& vertexFormat);

    [[nodiscard]]
    const VertexFormat& getVertexFormat() const;

    [[nodiscard]]
    const VkPipelineVertexInputStateCreateInfo& getVertexInputStateCreateInfo() const;

private:
    void createVertexInputState();

    VertexFormat vertexFormat;
    VkVertexInputBindingDescription vertexBindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
};
    
}
