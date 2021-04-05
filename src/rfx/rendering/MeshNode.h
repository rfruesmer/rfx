#pragma once

#include "rfx/rendering/RenderGraphNode.h"
#include "rfx/scene/Mesh.h"
#include "rfx/scene/MaterialShader.h"


namespace rfx {

class MeshNode : public RenderGraphNode
{
public:
    MeshNode(
        const MeshPtr& mesh,
        const MaterialPtr& material,
        MaterialShaderPtr shader);

    void record(const CommandBufferPtr& commandBuffer) const override;

    [[nodiscard]] bool isEmpty() const;

private:
    void bindObject(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderPtr& shader) const;

    MeshPtr mesh;
    std::vector<SubMesh> subMeshes;
    MaterialShaderPtr shader;
};

} // namespace rfx
