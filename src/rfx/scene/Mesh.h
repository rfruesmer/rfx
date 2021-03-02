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

    void setWorldTransform(const glm::mat4& transform);
    [[nodiscard]] const glm::mat4& getWorldTransform() const;

private:
    std::vector<SubMesh> subMeshes;
    glm::mat4 worldTransform_ { 1.0f };
};

} // namespace rfx
