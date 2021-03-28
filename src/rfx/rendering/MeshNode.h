#pragma once

#include "rfx/scene/Mesh.h"
#include "rfx/scene/MaterialShader.h"


namespace rfx {

class MeshNode
{
public:
    MeshNode(
        const MeshPtr& mesh,
        const MaterialPtr& material);

    void record(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderPtr& shader) const;

    [[nodiscard]] bool isEmpty() const;

private:
    void bindObject(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderPtr& shader) const;

    MeshPtr mesh;
    std::vector<SubMesh> subMeshes;
};

} // namespace rfx
