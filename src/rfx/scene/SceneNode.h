#pragma once

#include "rfx/scene/Mesh.h"

namespace rfx {

class SceneNode
{
public:
    explicit SceneNode(const std::shared_ptr<SceneNode>& parent);

    [[nodiscard]] const std::weak_ptr<SceneNode>& getParent() const;

    [[nodiscard]] const std::vector<std::shared_ptr<SceneNode>>& getChildren() const;
    void addChild(std::shared_ptr<SceneNode> child);

    [[nodiscard]] const glm::mat4& getLocalTransform() const;
    void setLocalTransform(const glm::mat4& localTransform);

    [[nodiscard]] const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;
    void addMesh(std::unique_ptr<Mesh> mesh);

private:
    std::weak_ptr<SceneNode> parent;
    std::vector<std::shared_ptr<SceneNode>> children;
    std::vector<std::unique_ptr<Mesh>> meshes;
    glm::mat4 localTransform_ { 1.0f };
};

} // namespace rfx