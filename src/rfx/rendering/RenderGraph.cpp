#include "rfx/pch.h"
#include "rfx/rendering/RenderGraph.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(
    const ModelPtr& model,
    const unordered_map<MaterialShaderPtr, vector<MaterialPtr>>& materialShaderMap)
{
    for (const auto& [shader, materials] : materialShaderMap) {
        add(shader, materials, model);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(
    const MaterialShaderPtr& shader,
    const vector<MaterialPtr>& materials,
    const ModelPtr& model)
{
    ShaderNode childNode(shader, materials, model);
    childNodes.push_back(childNode);
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::record(
    const CommandBufferPtr& commandBuffer,
    VkDescriptorSet sceneDescriptorSet)
{
    for (const auto& shaderNode : childNodes) {
        shaderNode.record(commandBuffer, sceneDescriptorSet);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::clear()
{
    childNodes.clear();
}

// ---------------------------------------------------------------------------------------------------------------------
