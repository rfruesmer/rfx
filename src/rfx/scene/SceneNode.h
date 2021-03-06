#pragma once

#include "rfx/scene/Mesh.h"

namespace rfx {

class SceneNode
{
public:
    explicit SceneNode(std::shared_ptr<SceneNode> parent);

    [[nodiscard]] SceneNode* getParent() const;

    void addChild(std::shared_ptr<SceneNode> child);
    void setChildren(const std::vector<std::shared_ptr<SceneNode>>& children);
    [[nodiscard]] const std::vector<std::shared_ptr<SceneNode>>& getChildren() const;

    void setLocalTransform(const glm::mat4& localTransform);
    [[nodiscard]] const glm::mat4& getLocalTransform() const;

    void setWorldTransform(const glm::mat4& worldTransform);
    [[nodiscard]] const glm::mat4& getWorldTransform() const;

    void addMesh(std::shared_ptr<Mesh> mesh);
    void setMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes);
    [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
    [[nodiscard]] uint32_t getMeshCount() const;

    void compile();

private:
    SceneNode* parent_ = nullptr;  // TODO: use weak_ptr?
    std::vector<std::shared_ptr<SceneNode>> children_;

    glm::mat4 localTransform_ { 1.0f };
    glm::mat4 worldTransform_ { 1.0f };
    std::vector<std::shared_ptr<Mesh>> meshes_;
};

}


