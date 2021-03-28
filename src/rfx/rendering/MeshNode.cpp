#include "rfx/pch.h"
#include "rfx/rendering/MeshNode.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

MeshNode::MeshNode(const MeshPtr& mesh, const MaterialPtr& material)
    : mesh(mesh)
{
    for (const auto& subMesh : mesh->getSubMeshes())
    {
        if (subMesh.material == material) {
            subMeshes.push_back(subMesh);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MeshNode::record(
    const CommandBufferPtr& commandBuffer,
    const MaterialShaderPtr& shader) const
{
    bindObject(commandBuffer, shader);

    for (const auto& subMesh : subMeshes) {
        commandBuffer->drawIndexed(subMesh.indexCount, subMesh.firstIndex);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MeshNode::bindObject(
    const CommandBufferPtr& commandBuffer,
    const MaterialShaderPtr& shader) const
{
    commandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->getPipelineLayout(),
        1,
        mesh->getDescriptorSet());
}

// ---------------------------------------------------------------------------------------------------------------------

bool MeshNode::isEmpty() const
{
    return subMeshes.empty();
}

// ---------------------------------------------------------------------------------------------------------------------

