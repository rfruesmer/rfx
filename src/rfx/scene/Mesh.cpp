#include "rfx/pch.h"
#include "rfx/scene/Mesh.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const vector<SubMesh>& Mesh::getSubMeshes() const
{
    return subMeshes;
}

// ---------------------------------------------------------------------------------------------------------------------

void Mesh::addSubMesh(const SubMesh& subMesh)
{
    subMeshes.push_back(subMesh);
}

// ---------------------------------------------------------------------------------------------------------------------

void Mesh::setWorldTransform(const mat4& transform)
{
    worldTransform_ = transform;
}

// ---------------------------------------------------------------------------------------------------------------------

const glm::mat4& Mesh::getWorldTransform() const
{
    return worldTransform_;
}

// ---------------------------------------------------------------------------------------------------------------------
