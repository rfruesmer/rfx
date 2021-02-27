#include "rfx/pch.h"
#include "rfx/scene/SceneNode.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SceneNode::SceneNode(const shared_ptr<SceneNode>& parent)
    : parent(parent) {}

// ---------------------------------------------------------------------------------------------------------------------

const weak_ptr<SceneNode>& SceneNode::getParent() const
{
    return parent;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<SceneNode>>& SceneNode::getChildren() const
{
    return children;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addChild(shared_ptr<SceneNode> child)
{
    children.push_back(move(child));
}

// ---------------------------------------------------------------------------------------------------------------------

const mat4& SceneNode::getLocalTransform() const
{
    return localTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::setLocalTransform(const mat4& localTransform)
{
    localTransform_ = localTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<unique_ptr<Mesh>>& SceneNode::getMeshes() const
{
    return meshes;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::addMesh(unique_ptr<Mesh> mesh)
{
    meshes.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------
