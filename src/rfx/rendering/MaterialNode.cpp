#include "rfx/pch.h"
#include "rfx/rendering/MaterialNode.h"

#include <utility>

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

MaterialNode::MaterialNode(
    const MaterialPtr& material,
    const ModelPtr& model)
        : material(material)
{
    add(material, model);
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialNode::add(
    const MaterialPtr& material,
    const ModelPtr& model)
{
    for (const auto& mesh : model->getMeshes())
    {
        MeshNode childNode(mesh, material);
        if (!childNode.isEmpty()) {
            childNodes.push_back(childNode);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialNode::record(
    const CommandBufferPtr& commandBuffer,
    const MaterialShaderPtr& shader) const
{
    bindMaterial(commandBuffer, shader);

    for (const auto& meshNode : childNodes) {
        meshNode.record(commandBuffer, shader);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MaterialNode::bindMaterial(
    const CommandBufferPtr& commandBuffer,
    const MaterialShaderPtr& shader) const
{
    commandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->getPipelineLayout(),
        2,
        material->getDescriptorSet());
}

// ---------------------------------------------------------------------------------------------------------------------
