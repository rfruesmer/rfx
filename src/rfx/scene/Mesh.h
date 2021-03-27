#pragma once

#include "rfx/scene/SubMesh.h"


namespace rfx {

class Mesh
{
public:
    [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;
    void addSubMesh(const SubMesh& subMesh);

    void setDescriptorSet(const VkDescriptorSet descriptorSet);
    VkDescriptorSet getDescriptorSet() const;

    void setDataBuffer(const BufferPtr& dataBuffer);
    const BufferPtr& getDataBuffer() const;

private:
    std::vector<SubMesh> subMeshes;
    VkDescriptorSet descriptorSet;
    BufferPtr dataBuffer;
};

} // namespace rfx
