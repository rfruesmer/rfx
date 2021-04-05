#pragma once

#include "rfx/rendering/RenderGraphNode.h"
#include "rfx/rendering/MeshNode.h"
#include "rfx/scene/Material.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"


namespace rfx {

class MaterialNode : public RenderGraphNode
{
public:
    MaterialNode(
        const MaterialPtr& material,
        const MaterialShaderPtr& shader,
        const ModelPtr& model);

    void record(const CommandBufferPtr& commandBuffer) const override;

private:
    void add(
        const MaterialPtr& material,
        const ModelPtr& model);

    void bindMaterial(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderPtr& shader) const;

    MaterialPtr material;
    MaterialShaderPtr shader;
    std::vector<MeshNode> childNodes;
};

} // namespace rfx
