#include "rfx/pch.h"
#include "rfx/scene/ModelNode.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

ModelNode::ModelNode(const NodePtr& parent)
    : Node(parent) {}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::setMeshes(const vector<MeshPtr>& meshes)
{
    meshes_ = meshes;
}

// ---------------------------------------------------------------------------------------------------------------------

void ModelNode::addMesh(MeshPtr mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<MeshPtr>& ModelNode::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ModelNode::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

