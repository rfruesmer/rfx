#pragma once

#include "rfx/scene/Mesh.h"
#include "rfx/scene/Light.h"

namespace rfx {

class ModelNode
{
public:
    explicit ModelNode(std::shared_ptr<ModelNode> parent);

    [[nodiscard]] ModelNode* getParent() const;

    void addChild(std::shared_ptr<ModelNode> child);
    void setChildren(const std::vector<std::shared_ptr<ModelNode>>& children);
    [[nodiscard]] const std::vector<std::shared_ptr<ModelNode>>& getChildren() const;

    void setLocalTransform(const glm::mat4& localTransform);
    [[nodiscard]] const glm::mat4& getLocalTransform() const;

    void setWorldTransform(const glm::mat4& worldTransform);
    [[nodiscard]] const glm::mat4& getWorldTransform() const;

    void addMesh(std::shared_ptr<Mesh> mesh);
    void setMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes);
    [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
    [[nodiscard]] uint32_t getMeshCount() const;

    void addLight(std::shared_ptr<Light> light);

    void compile();

private:
    void updateLights();

    ModelNode* parent_ = nullptr;  // TODO: use weak_ptr?
    std::vector<std::shared_ptr<ModelNode>> children_;

    glm::mat4 localTransform_ { 1.0f };
    glm::mat4 worldTransform_ { 1.0f };

    std::vector<std::shared_ptr<Mesh>> meshes_;
    std::vector<std::shared_ptr<Light>> lights_;
};

}


