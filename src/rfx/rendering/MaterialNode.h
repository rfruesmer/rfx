#pragma once

#include "rfx/rendering/MeshNode.h"
#include "rfx/scene/Material.h"
#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"


namespace rfx {

class MaterialNode
{
public:
    MaterialNode(
        const MaterialPtr& material,
        const ModelPtr& model);

    void record(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderPtr& shader) const;

private:
    void add(
        const MaterialPtr& material,
        const ModelPtr& model);

    void bindMaterial(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderPtr& shader) const;

    MaterialPtr material;
    std::vector<MeshNode> childNodes;
};

} // namespace rfx
