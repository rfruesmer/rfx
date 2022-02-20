#include "rfx/pch.h"
#include "rfx/scene/Node.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Node::Node(const NodePtr& parent)
    : parent_(parent.get()) {} // TODO: use weak_ptr?

// ---------------------------------------------------------------------------------------------------------------------

Node* Node::getParent() const
{
    return parent_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Node::addChild(NodePtr child)
{
    children_.push_back(move(child));
}

// ---------------------------------------------------------------------------------------------------------------------

void Node::setChildren(const vector<NodePtr>& children)
{
    children_ = children;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<NodePtr>& Node::getChildren() const
{
    return children_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Node::setLocalTransform(const mat4& localTransform)
{
    localTransform_ = localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& Node::getLocalTransform() const
{
    return localTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Node::setWorldTransform(const mat4& worldTransform)
{
    worldTransform_ = worldTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& Node::getWorldTransform() const
{
    return worldTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Node::compile()
{
    if (parent_) {
        worldTransform_ = parent_->getWorldTransform() * localTransform_;
    }

    update();

    for (const auto& child : children_) {
        child->compile();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
