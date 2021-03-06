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
    void createDescriptorPool() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;

    void cleanupSwapChain() override;

    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() override;
    [[nodiscard]] VkDescriptorSet getSceneDescriptorSet() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMaterialDescriptorSets() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const override;

protected:
//    struct SceneData {
//        glm::mat4 viewMatrix;
//        glm::mat4 projMatrix;
//        glm::vec4 lightPos;          // light position in eye coords
//        glm::vec4 La;                // Ambient light intensity
//        glm::vec4 Ld;                // Diffuse light intensity
//        glm::vec4 Ls;                // Specular light intensity
//    };

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
    VkDescriptorPool descriptorPool_ = nullptr;

    std::shared_ptr<Buffer> sceneDataBuffer_;
    std::vector<std::shared_ptr<Buffer>> materialDataBuffers_; // TODO: refactor to push constants
    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_; // TODO: refactor to sub-buffers

    std::shared_ptr<Scene> scene_;

private:
    VkDescriptorSetLayout sceneDescSetLayout_ = nullptr;
    VkDescriptorSet sceneDescSet_ {};

    VkDescriptorSetLayout materialDescSetLayout_ = nullptr;
    std::vector<VkDescriptorSet> materialDescSets_;

    VkDescriptorSetLayout meshDescSetLayout_ = nullptr;
    std::vector<VkDescriptorSet> meshDescSets_;
};

} // namespace rfx