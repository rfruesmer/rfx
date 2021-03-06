#include "rfx/pch.h"
#include "rfx/scene/SceneNode.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

SceneNode::SceneNode(std::shared_ptr<SceneNode> parent)
    : parent_(parent.get()) {} // TODO: use weak_ptr?

// ---------------------------------------------------------------------------------------------------------------------

SceneNode* SceneNode::getParent() const
{
    return parent_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addChild(shared_ptr<SceneNode> child)
{
    children_.push_back(move(child));
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setChildren(const vector<shared_ptr<SceneNode>>& children)
{
    children_ = children;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<SceneNode>>& SceneNode::getChildren() const
{
    return children_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setLocalTransform(const glm::mat4& localTransform)
{
    localTransform_ = localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const glm::mat4& SceneNode::getLocalTransform() const
{
    return localTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setWorldTransform(const glm::mat4& worldTransform)
{
    worldTransform_ = worldTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const glm::mat4& SceneNode::getWorldTransform() const
{
    return worldTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setMeshes(const vector<shared_ptr<Mesh>>& meshes)
{
    meshes_ = meshes;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addMesh(shared_ptr<Mesh> mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Mesh>>& SceneNode::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SceneNode::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::compile()
{
    if (parent_) {
        worldTransform_ = parent_->getWorldTransform() * localTransform_;
    }

    for (const auto& child : children_) {
        child->compile();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

