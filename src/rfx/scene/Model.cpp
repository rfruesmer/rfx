#include "rfx/pch.h"
#include "rfx/scene/Model.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Model::Model()
    : rootNode_(make_shared<ModelNode>(nullptr)) {}

// ---------------------------------------------------------------------------------------------------------------------

void Model::compile()
{
    rootNode_->compile();

    geometryNodes.clear();
    compile(rootNode_);
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::compile(const shared_ptr<ModelNode>& sceneNode)
{
    if (sceneNode->getMeshCount() > 0) {
        geometryNodes.push_back(sceneNode);
    }

    for (const auto& childNode : sceneNode->getChildren()) {
        compile(childNode);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<ModelNode>& Model::getRootNode() const
{
    return rootNode_;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<ModelNode>& Model::getGeometryNode(uint32_t index) const
{
    return geometryNodes[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<ModelNode>>& Model::getGeometryNodes() const
{
    return geometryNodes;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Model::getGeometryNodeCount() const
{
    return geometryNodes.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::setVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer)
{
    vertexBuffer_ = move(vertexBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexBuffer>& Model::getVertexBuffer() const
{
    return vertexBuffer_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::setIndexBuffer(shared_ptr<IndexBuffer> indexBuffer)
{
    indexBuffer_ = move(indexBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<IndexBuffer>& Model::getIndexBuffer() const
{
    return indexBuffer_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::addMesh(shared_ptr<Mesh> mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Mesh>& Model::getMesh(size_t index) const
{
    return meshes_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Mesh>>& Model::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Model::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::addMaterial(shared_ptr<Material> material)
{
    materials_.push_back(move(material));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Material>& Model::getMaterial(size_t index) const
{
    return materials_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Material>>& Model::getMaterials() const
{
    return materials_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Model::getMaterialCount() const
{
    return materials_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::addTexture(shared_ptr<Texture2D> texture)
{
    textures_.push_back(move(texture));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Model::getTexture(size_t index) const
{
    return textures_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Texture2D>>& Model::getTextures() const
{
    return textures_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Model::addLight(const shared_ptr<Light>& light)
{
    lights_.push_back(light);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Light>& Model::getLight(size_t index) const
{
    return lights_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Light>>& Model::getLights() const
{
    return lights_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Model::getLightCount() const
{
    return lights_.size();
}

// ---------------------------------------------------------------------------------------------------------------------
