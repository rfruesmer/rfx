#pragma once

#include "rfx/scene/Material.h"
#include "rfx/graphics/VertexBuffer.h"
#include "rfx/graphics/IndexBuffer.h"
#include "rfx/graphics/Texture2D.h"
#include "Mesh.h"


namespace rfx {

class Scene
{
public:
    void setVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer);
    [[nodiscard]] const std::shared_ptr<VertexBuffer>& getVertexBuffer() const;

    void setIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer);
    [[nodiscard]] const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;

    void addMesh(std::unique_ptr<Mesh> mesh);
    [[nodiscard]] const std::unique_ptr<Mesh>& getMesh(size_t index) const;
    [[nodiscard]] const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;
    uint32_t getMeshCount() const;

    void addMaterial(std::shared_ptr<Material> material);
    [[nodiscard]] const std::shared_ptr<Material>& getMaterial(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Material>>& getMaterials() const;

    void addTexture(std::shared_ptr<Texture2D> texture);
    [[nodiscard]] const std::shared_ptr<Texture2D>& getTexture(size_t index) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Texture2D>>& getTextures() const;

private:
    std::shared_ptr<VertexBuffer> vertexBuffer_;
    std::shared_ptr<IndexBuffer> indexBuffer_;

    std::vector<std::unique_ptr<Mesh>> meshes_;
    std::vector<std::shared_ptr<Material>> materials_;
    std::vector<std::shared_ptr<Texture2D>> textures_;
};

} // namespace rfx
