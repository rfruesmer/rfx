#include "rfx/pch.h"
#include "rfx/scene/Scene.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Scene::Scene()
    : rootNode_(make_shared<SceneNode>(nullptr)) {}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::compile()
{
    rootNode_->compile();

    geometryNodes.clear();
    compile(rootNode_);
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::compile(const shared_ptr<SceneNode>& sceneNode)
{
    if (sceneNode->getMeshCount() > 0) {
        geometryNodes.push_back(sceneNode);
    }

    for (const auto& childNode : sceneNode->getChildren()) {
        compile(childNode);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<SceneNode>& Scene::getRootNode() const
{
    return rootNode_;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<SceneNode>& Scene::getGeometryNode(uint32_t index) const
{
    return geometryNodes[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<SceneNode>>& Scene::getGeometryNodes() const
{
    return geometryNodes;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Scene::getGeometryNodeCount() const
{
    return geometryNodes.size();
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

void Scene::addMesh(shared_ptr<Mesh> mesh)
{
    meshes_.push_back(move(mesh));
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Mesh>& Scene::getMesh(size_t index) const
{
    return meshes_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Mesh>>& Scene::getMeshes() const
{
    return meshes_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Scene::getMeshCount() const
{
    return meshes_.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void Scene::addMaterial(shared_ptr<Material> material)
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

void Scene::addLight(const shared_ptr<Light>& light)
{
    lights_.push_back(light);
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Light>& Scene::getLight(size_t index) const
{
    return lights_[index];
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<shared_ptr<Light>>& Scene::getLights() const
{
    return lights_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Scene::getLightCount() const
{
    return lights_.size();
}

// ---------------------------------------------------------------------------------------------------------------------
