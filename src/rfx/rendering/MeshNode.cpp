#include "rfx/pch.h"
#include "rfx/rendering/MeshNode.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

MeshNode::MeshNode(const MeshPtr& mesh, const MaterialPtr& material)
    : mesh(mesh)
{
    for (const auto& subMesh : mesh->getSubMeshes())
    {
        if (subMesh.material == material) {
            subMeshes.push_back(subMesh);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const MeshPtr& MeshNode::getMesh() const
{
    return mesh;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<SubMesh>& MeshNode::getSubMeshes() const
{
    return subMeshes;
}

// ---------------------------------------------------------------------------------------------------------------------

bool MeshNode::isEmpty() const
{
    return subMeshes.empty();
}

// ---------------------------------------------------------------------------------------------------------------------

