#pragma once

#include "rfx/scene/SubMesh.h"


namespace rfx {

class Mesh
{
public:
    [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;
    void addSubMesh(const SubMesh& subMesh);

private:
    std::vector<SubMesh> subMeshes;
};

} // namespace rfx
