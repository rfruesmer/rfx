#include "rfx/pch.h"
#include "rfx/rendering/ShaderNode.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

ShaderNode::ShaderNode(
    MaterialShaderPtr shader,
    const vector<MaterialPtr>& materials,
    const ModelPtr& model)
        : shader(move(shader))
{
    add(materials, model);
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderNode::add(const vector<MaterialPtr>& materials, const ModelPtr& model)
{
    for (const auto& material : materials) {
        MaterialNode childNode(material, model);
        childNodes.push_back(childNode);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderNode::record(
    const CommandBufferPtr& commandBuffer,
    VkDescriptorSet sceneDescriptorSet) const
{
    bindShader(commandBuffer, sceneDescriptorSet);

    for (const auto& materialNode : childNodes) {
        materialNode.record(commandBuffer, shader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderNode::bindShader(
    const CommandBufferPtr& commandBuffer,
    VkDescriptorSet sceneDescriptorSet) const
{
    commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getPipeline());

    commandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->getPipelineLayout(),
        0,
        sceneDescriptorSet);

    commandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->getPipelineLayout(),
        1,
        shader->getShaderDescriptorSet());
}

// ---------------------------------------------------------------------------------------------------------------------
