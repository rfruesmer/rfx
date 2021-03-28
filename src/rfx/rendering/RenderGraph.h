#pragma once

#include "rfx/scene/Model.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/rendering/ShaderNode.h"


namespace rfx {

/**
 *  TODO: support for multiple models
 */
class RenderGraph
{
public:
    void add(
        const ModelPtr& model,
        const std::unordered_map<MaterialShaderPtr, std::vector<MaterialPtr>>& materialShaderMap);

    void record(
        const CommandBufferPtr& commandBuffer,
        VkDescriptorSet sceneDescriptorSet);

    void clear();

private:
    void add(
        const MaterialShaderPtr& shader,
        const std::vector<MaterialPtr>& materials,
        const ModelPtr& model);


    std::vector<ShaderNode> childNodes;
};

using RenderGraphPtr = std::shared_ptr<RenderGraph>;

} // namespace rfx
