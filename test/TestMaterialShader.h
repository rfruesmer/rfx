#pragma once

#include "rfx/scene/MaterialShader.h"
#include "rfx/scene/Model.h"


namespace rfx {

class TestMaterialShader : public MaterialShader
{
public:
    TestMaterialShader(
        std::shared_ptr<GraphicsDevice> graphicsDevice,
        std::shared_ptr<Model> scene);


    void createUniformBuffers() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets(VkDescriptorPool descriptorPool) override;

    void cleanupSwapChain() override;

    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const override;

protected:
    struct MeshData {
        glm::mat4 modelMatrix;
    };

    void createMeshDataBuffers();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets(VkDescriptorPool descriptorPool);


    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_;

    std::shared_ptr<Model> scene_;

private:
    enum DescriptorType {
        MESH,
        QUANTITY
    };

    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorBufferInfo* descriptorBufferInfo);


    std::vector<VkDescriptorSetLayout> descriptorSetLayouts_ { DescriptorType::QUANTITY };

    std::vector<VkDescriptorSet> meshDescriptorSets_;
};

} // namespace rfx