#pragma once

#include "rfx/scene/Effect.h"
#include "rfx/scene/Scene.h"
//#include "rfx/scene/PointLight.h"
#include "rfx/graphics/GraphicsDevice.h"


namespace rfx {

class TestEffect : public Effect
{
public:
    TestEffect(
        std::shared_ptr<GraphicsDevice> graphicsDevice,
        std::shared_ptr<Scene> scene);

    [[nodiscard]] virtual size_t getSceneDataSize() const = 0;

    void createUniformBuffers() override;
    void createDescriptorPools() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;

    void cleanupSwapChain() override;

    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() override;
    [[nodiscard]] VkDescriptorSet getSceneDescriptorSet() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMaterialDescriptorSets() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const override;

protected:
    struct MeshData {
        glm::mat4 modelMatrix;
    };

    struct MaterialData {
        glm::vec4 baseColor;
        float shininess;
    };

    void createSceneDataBuffer();
    void createMeshDataBuffers();
    void createMaterialDataBuffers();

    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createMaterialDescriptorSetLayout();
    void createMaterialDescriptorSets();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets();

    std::shared_ptr<GraphicsDevice> graphicsDevice_;

    std::shared_ptr<Buffer> sceneDataBuffer_;
    std::vector<std::shared_ptr<Buffer>> materialDataBuffers_; // TODO: refactor to push constants

    // TODO: rename to nodeDataBuffers ??
    // TODO: refactor to sub-buffers
    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_;

    std::shared_ptr<Scene> scene_;

private:
    enum DescriptorType {
        SCENE,
        MESH,
        MATERIAL,
        QUANTITY
    };

    VkDescriptorPool createDescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);

    std::vector<VkDescriptorPool> descriptorPools_ { DescriptorType::QUANTITY };
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts_ { DescriptorType::QUANTITY };

    VkDescriptorSet sceneDescriptorSet_ {};
    std::vector<VkDescriptorSet> materialDescriptorSets_;
    std::vector<VkDescriptorSet> meshDescriptorSets_;
};

} // namespace rfx