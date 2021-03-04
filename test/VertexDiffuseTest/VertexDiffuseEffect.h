#pragma once

#include "rfx/scene/Effect.h"
#include "rfx/scene/Scene.h"
#include "rfx/graphics/GraphicsDevice.h"


namespace rfx {

class VertexDiffuseEffect : public Effect
{
public:
    static inline const VertexFormat VERTEX_FORMAT {
        VertexFormat::COORDINATES | VertexFormat::NORMALS
    };

    VertexDiffuseEffect(
        std::shared_ptr<GraphicsDevice> graphicsDevice,
        std::shared_ptr<Scene> scene);
    ~VertexDiffuseEffect() override;

    void createUniformBuffers() override;
    void createDescriptorPool() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;

    [[nodiscard]] VertexFormat getVertexFormat() const override { return VERTEX_FORMAT; };
    [[nodiscard]] std::string getVertexShaderFileName() const override;
    [[nodiscard]] std::string getFragmentShaderFileName() const override;

    [[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() override;
    [[nodiscard]] VkDescriptorSet getSceneDescriptorSet() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMaterialDescriptorSets() const override;
    [[nodiscard]] const std::vector<VkDescriptorSet>& getMeshDescriptorSets() const override;

    void setProjectionMatrix(const glm::mat4& projection) override;
    void setViewMatrix(const glm::mat4& viewMatrix) override;
    void setLightPosition(const glm::vec3& lightPosition);
    void setLightDiffuseColor(const glm::vec3& diffuseColor);

    void updateSceneDataBuffer() override;

    void cleanupSwapChain() override;

private:
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

    std::shared_ptr<Scene> scene_;
    glm::vec3 lightPosition_ { 0.0f };

    VkDescriptorSetLayout sceneDescSetLayout_ = nullptr;
    VkDescriptorSet sceneDescSet_ {};
    std::shared_ptr<Buffer> sceneDataBuffer_;
    SceneData sceneData_ {};

    VkDescriptorSetLayout materialDescSetLayout_ = nullptr;
    std::vector<VkDescriptorSet> materialDescSets_;
    std::vector<std::shared_ptr<Buffer>> materialDataBuffers_; // TODO: refactor to push constants

    VkDescriptorSetLayout meshDescSetLayout_ = nullptr;
    std::vector<VkDescriptorSet> meshDescSets_;
    std::vector<std::shared_ptr<Buffer>> meshDataBuffers_; // TODO: refactor to sub-buffers
};

} // namespace rfx