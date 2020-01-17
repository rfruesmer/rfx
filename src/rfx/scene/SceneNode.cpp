#include "rfx/pch.h"
#include "rfx/scene/SceneNode.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

SceneNode::SceneNode(const SceneNode* parent)
    : parent(parent) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& SceneNode::getName() const
{
    return name;
}

// ---------------------------------------------------------------------------------------------------------------------

const SceneNode* SceneNode::getParent() const
{
    return parent;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneNode::attach(const std::shared_ptr<Mesh>& mesh)
{
    meshes.push_back(mesh);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::shared_ptr<Mesh>>& SceneNode::getMeshes() const
{
    return meshes;
}

// ---------------------------------------------------------------------------------------------------------------------
