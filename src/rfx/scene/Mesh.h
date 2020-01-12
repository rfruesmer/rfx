#pragma once

#include "rfx/graphics/GraphicsDevice.h"

namespace rfx
{

class Mesh
{
public:
    Mesh(const std::shared_ptr<GraphicsDevice>& graphicsDevice,
         const std::shared_ptr<VertexBuffer>& vertexBuffer, 
         const std::shared_ptr<IndexBuffer>& indexBuffer);
    ~Mesh();

    const std::shared_ptr<VertexBuffer>& getVertexBuffer() const;
    const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;

    void setVertexShader(const VkPipelineShaderStageCreateInfo& vertexShaderStage);
    void setFragmentShader(const VkPipelineShaderStageCreateInfo& fragmentShaderStage);
    const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

private:
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
};
    
}
