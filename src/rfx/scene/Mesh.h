#pragma once

#include "rfx/scene/SubMesh.h"


namespace rfx {

class Mesh
{
public:
    void addSubMesh(const SubMesh& subMesh);
    [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;

    void setDescriptorSet(VkDescriptorSet descriptorSet);
    [[nodiscard]] VkDescriptorSet getDescriptorSet() const;

    void setDataBuffer(const BufferPtr& dataBuffer);
    [[nodiscard]] const BufferPtr& getDataBuffer() const;

private:
    std::vector<SubMesh> subMeshes;
    VkDescriptorSet descriptorSet;
    BufferPtr dataBuffer;
};

using MeshPtr = std::shared_ptr<Mesh>;

} // namespace rfx
