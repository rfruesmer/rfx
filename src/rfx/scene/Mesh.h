#pragma once

namespace rfx {

class SubMesh
{
public:
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t materialIndex;
};

class Mesh
{
public:
    [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;
    void addSubMesh(const SubMesh& subMesh);

private:
    std::vector<SubMesh> subMeshes;
};

} // namespace rfx
