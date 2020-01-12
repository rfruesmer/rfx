#include "rfx/pch.h"
#include "rfx/scene/Mesh.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Mesh::Mesh(const std::shared_ptr<GraphicsDevice>& graphicsDevice,
           const shared_ptr<VertexBuffer>& vertexBuffer, 
           const shared_ptr<IndexBuffer>& indexBuffer)
    : graphicsDevice(graphicsDevice),
      vertexBuffer(vertexBuffer),
      indexBuffer(indexBuffer)
{
    shaderStages.push_back({});
    shaderStages.push_back({});
}

// ---------------------------------------------------------------------------------------------------------------------

Mesh::~Mesh()
{
    for (auto& shaderStage : shaderStages) {
        graphicsDevice->destroyShaderModule(shaderStage.module);
    }
    shaderStages.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexBuffer>& Mesh::getVertexBuffer() const
{
    return vertexBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<IndexBuffer>& Mesh::getIndexBuffer() const
{
    return indexBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void Mesh::setVertexShader(const VkPipelineShaderStageCreateInfo& vertexShaderStage)
{
    shaderStages[0] = vertexShaderStage;
}

// ---------------------------------------------------------------------------------------------------------------------

void Mesh::setFragmentShader(const VkPipelineShaderStageCreateInfo& fragmentShaderStage)
{
    shaderStages[1] = fragmentShaderStage;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkPipelineShaderStageCreateInfo>& Mesh::getShaderStages() const
{
    return shaderStages;
}

// ---------------------------------------------------------------------------------------------------------------------
