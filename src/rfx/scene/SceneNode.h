#pragma once

#include "rfx/scene/Mesh.h"
#include "rfx/scene/Transform.h"

namespace rfx
{
    
class SceneNode
{
public:
    SceneNode() = default;
    explicit SceneNode(const std::string& name);

    [[nodiscard]] const std::string& getName() const;

    void setParent(const SceneNode* parent);
    const SceneNode* getParent() const;

    void updateWorldTransform();
    const Transform& getWorldTransform() const;

    Transform& getLocalTransform();

    void attach(std::unique_ptr<SceneNode>& childNode);
    [[nodiscard]] const std::vector<std::unique_ptr<SceneNode>>& getChildNodes() const;

    void attach(const std::shared_ptr<Mesh>& mesh);
    [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;

private:
    std::string name;
    const SceneNode* parent = nullptr;
    Transform localTransform;
    Transform worldTransform;
    std::vector<std::unique_ptr<SceneNode>> childNodes;
    std::vector<std::shared_ptr<Mesh>> meshes;
};

} // namespace rfx;
