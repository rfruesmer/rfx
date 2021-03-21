#pragma once

#include "rfx/scene/Material.h"
#include "rfx/scene/ModelNode.h"
#include "rfx/scene/Mesh.h"
#include "rfx/scene/Light.h"
#include "rfx/graphics/VertexBuffer.h"
#include "rfx/graphics/IndexBuffer.h"
#include "rfx/graphics/Texture2D.h"


namespace rfx {

class Model
{
public:
    Model();

    void compile();

    [[nodiscard]] const std::shared_ptr<ModelNode>& getRootNode() const;

    [[nodiscard]] const std::shared_ptr<ModelNode>& getGeometryNode(uint32_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<ModelNode>>& getGeometryNodes() const;
    [[nodiscard]] uint32_t getGeometryNodeCount() const;

    void setVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer);
    [[nodiscard]] const std::shared_ptr<VertexBuffer>& getVertexBuffer() const;

    void setIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer);
    [[nodiscard]] const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;

    void addMesh(std::shared_ptr<Mesh> mesh);
    [[nodiscard]] const std::shared_ptr<Mesh>& getMesh(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& getMeshes() const;
    [[nodiscard]] uint32_t getMeshCount() const;

    void addMaterial(std::shared_ptr<Material> material);
    [[nodiscard]] const std::shared_ptr<Material>& getMaterial(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Material>>& getMaterials() const;
    [[nodiscard]] uint32_t getMaterialCount() const;

    void addTexture(std::shared_ptr<Texture2D> texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getTexture(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Texture2D>>& getTextures() const;

    void addLight(const std::shared_ptr<Light>& light);
    [[nodiscard]] const std::shared_ptr<Light>& getLight(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Light>>& getLights() const;
    [[nodiscard]] uint32_t getLightCount() const;

private:
    void compile(const std::shared_ptr<ModelNode>& sceneNode);

    std::shared_ptr<ModelNode> rootNode_;
    std::vector<std::shared_ptr<ModelNode>> geometryNodes;

    std::shared_ptr<VertexBuffer> vertexBuffer_;
    std::shared_ptr<IndexBuffer> indexBuffer_;

    std::vector<std::shared_ptr<Mesh>> meshes_;
    std::vector<std::shared_ptr<Material>> materials_;
    std::vector<std::shared_ptr<Texture2D>> textures_;
    std::vector<std::shared_ptr<Light>> lights_;
};

} // namespace rfx
