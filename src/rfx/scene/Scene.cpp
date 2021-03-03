#include "rfx/pch.h"
#include "rfx/scene/Scene.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void Scene::setVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer)
{
    vertexBuffer_ = move(vertexBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<VertexBuffer>& Scene::getVertexBuffer() const
{
    return vertexBuffer_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::setIndexBuffer(shared_ptr<IndexBuffer> indexBuffer)
{
    indexBuffer_ = move(indexBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<IndexBuffer>& Scene::getIndexBuffer() const
{
    return indexBuffer_;
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::addMesh(unique_ptr<Mesh> mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const unique_ptr<Mesh>& Scene::getMesh(size_t index) const
{
    return meshes_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<unique_ptr<Mesh>>& Scene::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Scene::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::addMaterial(std::shared_ptr<Material> material)
{
    materials_.push_back(move(material));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Material>& Scene::getMaterial(size_t index) const
{
    return materials_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Material>>& Scene::getMaterials() const
{
    return materials_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Scene::getMaterialCount() const
{
    return materials_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::addTexture(shared_ptr<Texture2D> texture)
{
    textures_.push_back(move(texture));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Scene::getTexture(size_t index) const
{
    return textures_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Texture2D>>& Scene::getTextures() const
{
    return textures_;
}

// ---------------------------------------------------------------------------------------------------------------------
