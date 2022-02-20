#pragma once

#include "rfx/scene/Node.h"
#include "rfx/scene/Mesh.h"

namespace rfx {

class ModelNode : public Node
{
public:
    explicit ModelNode(const NodePtr& parent);


    void addMesh(MeshPtr mesh);
    void setMeshes(const std::vector<MeshPtr>& meshes);
    [[nodiscard]] const std::vector<MeshPtr>& getMeshes() const;
    [[nodiscard]] uint32_t getMeshCount() const;

private:
    std::vector<MeshPtr> meshes_;
};

} // namespace rfx
