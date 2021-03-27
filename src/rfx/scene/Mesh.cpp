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

void Mesh::setDescriptorSet(VkDescriptorSet descriptorSet)
{
    this->descriptorSet = descriptorSet;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet Mesh::getDescriptorSet() const
{
    return descriptorSet;
}

// ---------------------------------------------------------------------------------------------------------------------

void Mesh::setDataBuffer(const BufferPtr& dataBuffer)
{
    this->dataBuffer = dataBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

const BufferPtr& Mesh::getDataBuffer() const
{
    return dataBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

