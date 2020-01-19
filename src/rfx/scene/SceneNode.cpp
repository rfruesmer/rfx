#include "rfx/pch.h"
#include "rfx/scene/SceneNode.h"
#include "rfx/core/Algorithm.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

SceneNode::SceneNode(const string& name)
    : name(name) {}

// ---------------------------------------------------------------------------------------------------------------------

const string& SceneNode::getName() const
{
    return name;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setParent(const SceneNode* parent)
{
    RFX_CHECK_STATE(this->parent == nullptr, "This node already has a parent");

    this->parent = parent;
}

// ---------------------------------------------------------------------------------------------------------------------

const SceneNode* SceneNode::getParent() const
{
    return parent;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::updateWorldTransform()
{
    if (parent != nullptr) {
        const Transform& parentWorldTransform = parent->getWorldTransform();
        worldTransform.setTranslation(parentWorldTransform.getTranslation() + localTransform.getTranslation());
        worldTransform.setScale(parentWorldTransform.getScale() * localTransform.getScale());
        worldTransform.setRotation(parentWorldTransform.getRotation() + localTransform.getRotation());
        worldTransform.update();
    }
    else {
        worldTransform = localTransform;
    }

    for (const auto& childNode : childNodes) {
        childNode->updateWorldTransform();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const Transform& SceneNode::getWorldTransform() const
{
    return worldTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

Transform& SceneNode::getLocalTransform()
{
    return localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::attach(unique_ptr<SceneNode>& childNode)
{
    RFX_CHECK_ARGUMENT(childNode->getParent() == nullptr || childNode->getParent() == this,
        "Invalid child node");

    if (!contains(childNodes, childNode)) {
        childNodes.push_back(std::move(childNode));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<unique_ptr<SceneNode>>& SceneNode::getChildNodes() const
{
    return childNodes;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::attach(const shared_ptr<Mesh>& mesh)
{
    meshes.push_back(mesh);
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Mesh>>& SceneNode::getMeshes() const
{
    return meshes;
}

// ---------------------------------------------------------------------------------------------------------------------

