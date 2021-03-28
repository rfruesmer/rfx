#pragma once

#include "rfx/rendering/MaterialNode.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"

namespace rfx {

class ShaderNode
{
public:
    ShaderNode(
        MaterialShaderPtr shader,
        const std::vector<MaterialPtr>& materials,
        const ModelPtr& model);

    void record(
        const CommandBufferPtr& commandBuffer,
        VkDescriptorSet sceneDescriptorSet) const;

private:
    void add(const std::vector<MaterialPtr>& materials, const ModelPtr& model);

    void bindShader(
        const CommandBufferPtr& commandBuffer,
        VkDescriptorSet sceneDescriptorSet) const;

    MaterialShaderPtr shader;
    std::vector<MaterialNode> childNodes;
};

} // namespace rfx
