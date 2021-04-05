#include "rfx/pch.h"
#include "rfx/rendering/ShaderNode.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

ShaderNode::ShaderNode(
    MaterialShaderPtr shader,
    const vector<MaterialPtr>& materials,
    const ModelPtr& model,
    VkDescriptorSet sceneDescriptorSet)
        : shader(move(shader)),
          sceneDescriptorSet(sceneDescriptorSet)
{
    add(materials, model);
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderNode::add(
    const vector<MaterialPtr>& materials,
    const ModelPtr& model)
{
    for (const auto& material : materials) {
        MaterialNode childNode(material, shader, model);
        childNodes.push_back(childNode);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderNode::record(const CommandBufferPtr& commandBuffer) const
{
    bindShader(commandBuffer);

    for (const auto& materialNode : childNodes) {
        materialNode.record(commandBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderNode::bindShader(const CommandBufferPtr& commandBuffer) const
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
