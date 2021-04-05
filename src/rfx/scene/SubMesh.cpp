#include "rfx/pch.h"
#include "rfx/scene/SubMesh.h"


using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SubMesh::SubMesh(
    uint32_t firstIndex,
    uint32_t indexCount,
    MaterialPtr material)
    : firstIndex(firstIndex),
      indexCount(indexCount),
      material(move(material)) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SubMesh::getFirstIndex() const
{
    return firstIndex;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SubMesh::getIndexCount() const
{
    return indexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubMesh::setMaterial(const MaterialPtr& material)
{
    SubMesh::material = material;
}

// ---------------------------------------------------------------------------------------------------------------------

const MaterialPtr& SubMesh::getMaterial() const
{
    return material;
}

// ---------------------------------------------------------------------------------------------------------------------
