#pragma once

#include "rfx/scene/SceneNode.h"
#include "rfx/scene/Material.h"
#include "rfx/graphics/VertexBuffer.h"
#include "rfx/graphics/IndexBuffer.h"
#include "rfx/graphics/Texture2D.h"


namespace rfx {

class Scene
{
public:
//    Scene(
//        std::vector<std::shared_ptr<SceneNode>> nodes,
//        std::shared_ptr<VertexBuffer> vertexBuffer,
//        std::shared_ptr<IndexBuffer> indexBuffer,
//        std::vector<std::shared_ptr<Texture2D>> textures);

    void setRootNode(std::shared_ptr<SceneNode> rootNode);
    [[nodiscard]] const std::shared_ptr<SceneNode>& getRootNode() const;

    void setVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer);
    [[nodiscard]] const std::shared_ptr<VertexBuffer>& getVertexBuffer() const;

    void setIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer);
    [[nodiscard]] const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;

    void addMaterial(std::shared_ptr<Material> material);
    [[nodiscard]] const std::shared_ptr<Material>& getMaterial(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Material>>& getMaterials() const;

    void addTexture(std::shared_ptr<Texture2D> texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getTexture(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Texture2D>>& getTextures() const;

private:
    std::shared_ptr<SceneNode> rootNode_;
    std::shared_ptr<VertexBuffer> vertexBuffer_;
    std::shared_ptr<IndexBuffer> indexBuffer_;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<Texture2D>> textures;
};

} // namespace rfx
