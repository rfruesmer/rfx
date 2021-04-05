#pragma once

#include "rfx/rendering/RenderGraphNode.h"
#include "rfx/rendering/MaterialNode.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"


namespace rfx {

class ShaderNode : public RenderGraphNode
{
public:
    ShaderNode(
        MaterialShaderPtr shader,
        const std::vector<MaterialPtr>& materials,
        const ModelPtr& model,
        VkDescriptorSet sceneDescriptorSet);

    void record(const CommandBufferPtr& commandBuffer) const override;

private:
    void add(const std::vector<MaterialPtr>& materials, const ModelPtr& model);

    void bindShader(const CommandBufferPtr& commandBuffer) const;

    MaterialShaderPtr shader;
    std::vector<MaterialNode> childNodes;
    VkDescriptorSet sceneDescriptorSet = VK_NULL_HANDLE;
};

} // namespace rfx
