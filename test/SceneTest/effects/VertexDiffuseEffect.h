#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class VertexDiffuseEffect
{
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec4 lightPos;          // light position in eye coords
        glm::vec4 Ld;                // diffuse light intensity
    };

    struct MeshData {
        glm::mat4 modelMatrix;
    };

    struct MaterialData {
        glm::vec4 Kd;                // diffuse reflectivity (material property)
    };

public:
    VertexDiffuseEffect(
        std::shared_ptr<GraphicsDevice> graphicsDevice,
        std::shared_ptr<Scene> scene);

    ~VertexDiffuseEffect();

    void createUniformBuffers();
    void setDescriptorPool(VkDescriptorPool descriptorPool);
    void createDescriptorSetLayouts();
    void createDescriptorSets();

    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();
    [[nodiscard]] VkDescriptorSet getSceneDescSet() const;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMaterialDescSets() const;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescSets() const;

    void setProjection(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLightPosition(const glm::vec3& lightPosition);
    void setLightDiffuseColor(const glm::vec3& diffuseColor);

    void updateSceneDataMemory();

    void cleanupSwapChain();

private:
    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createMaterialDescriptorSetLayout();
    void createMaterialDescriptorSets();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets();


    std::shared_ptr<GraphicsDevice> graphicsDevice_;
    VkDescriptorPool descriptorPool_;

    std::shared_ptr<Scene> scene_;
    glm::vec3 lightPosition_ { 0.0f };

    VkDescriptorSetLayout sceneDescSetLayout_;
    VkDescriptorSet sceneDescSet_ {};
    std::shared_ptr<Buffer> sceneDataBuffer_;
    SceneData sceneData_ {};

    VkDescriptorSetLayout materialDescSetLayout_;
    std::vector<VkDescriptorSet> materialDescSets_;
    std::vector<std::shared_ptr<Buffer>> materialDataBuffers_; // TODO: refactor to push constants

    VkDescriptorSetLayout meshDescSetLayout_;
    std::vector<VkDescriptorSet> meshDescSets_;
    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_; // TODO: refactor to sub-buffers
    void createSceneDataBuffer();

    void createMeshDataBuffers();

    void createMaterialDataBuffers();
};

} // namespace rfx