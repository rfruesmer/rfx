#pragma once

namespace rfx {

class Node;
using NodePtr = std::shared_ptr<Node>;

class Node
{
public:
    explicit Node(const NodePtr& parent);
    virtual ~Node() = default;

    [[nodiscard]] Node* getParent() const;

    void addChild(NodePtr child);
    void setChildren(const std::vector<NodePtr>& children);
    [[nodiscard]] const std::vector<NodePtr>& getChildren() const;

    void setLocalTransform(const glm::mat4& localTransform);
    [[nodiscard]] const glm::mat4& getLocalTransform() const;

    void setWorldTransform(const glm::mat4& worldTransform);
    [[nodiscard]] const glm::mat4& getWorldTransform() const;

    void compile();

protected:
    Node* parent_ = nullptr;  // TODO: use weak_ptr?
    std::vector<NodePtr> children_;

    glm::mat4 localTransform_ { 1.0f };
    glm::mat4 worldTransform_ { 1.0f };

private:
    virtual void update() {}
};

} // namespace rfx
