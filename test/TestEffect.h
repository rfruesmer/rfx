#pragma once

#include "rfx/scene/Effect.h"
#include "rfx/scene/Model.h"


namespace rfx {

class TestEffect : public Effect
{
public:
    TestEffect(
        std::shared_ptr<GraphicsDevice> graphicsDevice,
        std::shared_ptr<Model> scene);


    void createUniformBuffers() override;
    void createDescriptorPools() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;

    void cleanupSwapChain() override;

    [[nodiscard]] VkDescriptorPool getDescriptorPool() const;
    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const override;
    [[nodiscard]] VkDescriptorSet getSceneDescriptorSet() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const override;

protected:
    struct MeshData {
        glm::mat4 modelMatrix;
    };

    void createSceneDataBuffer();
    void createMeshDataBuffers();

    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets();

    [[nodiscard]] virtual size_t getSceneDataSize() const = 0;


    std::shared_ptr<Buffer> sceneDataBuffer_;

    // TODO: rename to nodeDataBuffers ??
    // TODO: refactor to sub-buffers
    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_;

    std::shared_ptr<Model> scene_;

private:
    enum DescriptorType {
        SCENE,
        MESH,
        QUANTITY
    };

    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorBufferInfo* descriptorBufferInfo);

    VkDescriptorPool createDescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);

    VkDescriptorPool descriptorPool_ {}; // TODO: move to application ?!
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts_ { DescriptorType::QUANTITY };

    VkDescriptorSet sceneDescriptorSet_ {};
    std::vector<VkDescriptorSet> meshDescriptorSets_;
};

} // namespace rfx