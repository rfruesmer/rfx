#pragma once

#include "rfx/scene/Mesh.h"

namespace rfx
{
    
class SceneNode
{
public:
    SceneNode() = default;
    explicit SceneNode(const SceneNode* parent);

    [[nodiscard]] const std::string& getName() const;
    const SceneNode* getParent() const;

    void attach(const std::shared_ptr<Mesh>& mesh);
    [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;

private:
    std::string name;
    const SceneNode* parent = nullptr;
    glm::mat4 localTransform = glm::mat4(1.0F);
    glm::mat4 worldTransform = glm::mat4(1.0F);
    std::vector<std::shared_ptr<Mesh>> meshes;
};

} // namespace rfx;
