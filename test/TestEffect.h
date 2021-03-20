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


    void createUniformBuffers() override;
    void createDescriptorPools() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;

    void cleanupSwapChain() override;

    [[nodiscard]] VertexFormat getVertexFormat() const override;
    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const override;
    [[nodiscard]] VkDescriptorSet getSceneDescriptorSet() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMaterialDescriptorSets() const override;

protected:
    struct MeshData {
        glm::mat4 modelMatrix;
    };

    void createSceneDataBuffer();
    void createMeshDataBuffers();
    virtual void createMaterialDataBuffers();

    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createMaterialDescriptorSetLayout();
    void createMaterialDescriptorSets();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets();

    [[nodiscard]] virtual size_t getSceneDataSize() const = 0;

    std::shared_ptr<GraphicsDevice> graphicsDevice_;

    std::shared_ptr<Buffer> sceneDataBuffer_;
    std::vector<std::shared_ptr<Buffer>> materialDataBuffers_; // TODO: refactor to push constants

    // TODO: rename to nodeDataBuffers ??
    // TODO: refactor to sub-buffers
    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_;

    std::shared_ptr<Scene> scene_;

private:
    struct MaterialData {
        glm::vec4 baseColor;
        glm::vec3 specular;
        float shininess = 0.0f;
    };

    enum DescriptorType {
        SCENE,
        MESH,
        MATERIAL,
        QUANTITY
    };

    VkDescriptorPool createDescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorImageInfo* descriptorImageInfo);
    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorBufferInfo* descriptorBufferInfo);


    VkDescriptorPool descriptorPool_ {};
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts_ { DescriptorType::QUANTITY };

    VkDescriptorSet sceneDescriptorSet_ {};
    std::vector<VkDescriptorSet> meshDescriptorSets_;
    std::vector<VkDescriptorSet> materialDescriptorSets_;
};

} // namespace rfx