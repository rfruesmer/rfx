#include "rfx/pch.h"
#include "rfx/scene/Scene.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

void Scene::setRootNode(shared_ptr<SceneNode> rootNode)
{
    rootNode_ = move(rootNode);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<SceneNode>& Scene::getRootNode() const
{
    return rootNode_;
}

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

void Scene::addMaterial(std::shared_ptr<Material> material)
{
    materials.push_back(move(material));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Material>& Scene::getMaterial(size_t index) const
{
    return materials[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Material>>& Scene::getMaterials() const
{
    return materials;
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::addTexture(shared_ptr<Texture2D> texture)
{
    textures.push_back(move(texture));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Texture2D>& Scene::getTexture(size_t index) const
{
    return textures[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Texture2D>>& Scene::getTextures() const
{
    return textures;
}

// ---------------------------------------------------------------------------------------------------------------------
