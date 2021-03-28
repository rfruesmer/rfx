#pragma once

#include "rfx/scene/Mesh.h"


namespace rfx {

class MeshNode
{
public:
    MeshNode(const MeshPtr& mesh, const MaterialPtr& material);

    [[nodiscard]] const MeshPtr& getMesh() const;
    [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;
    [[nodiscard]] bool isEmpty() const;

private:
    MeshPtr mesh;
    std::vector<SubMesh> subMeshes;
};

} // namespace rfx
