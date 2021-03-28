#pragma once

#include "rfx/application/Application.h"
#include "rfx/rendering/RenderGraph.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/FlyCamera.h"
#include "rfx/scene/MaterialShaderFactory.h"
#include "rfx/scene/PointLight.h"


namespace rfx {

class TestApplication : public Application
{
protected:
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec3 lightPos;          // light position in eye coords
        [[maybe_unused]] float pad;
        glm::vec4 La;
        glm::vec4 Ld;
        glm::vec4 Ls;
    };

    struct MeshData {
        glm::mat4 modelMatrix;
    };

    virtual void createShaders() = 0;

    void createDescriptorPool();

    void createSceneResources();
    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createSceneDataBuffer();
    virtual void updateSceneData(float deltaTime) = 0;
    void updateSceneDataBuffer();

    virtual void createMeshResources() = 0;
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets(const ModelPtr& model);
    void createMeshDataBuffers(const ModelPtr& model);

    virtual void createPipelines() = 0;
    virtual void createCommandBuffers() = 0;

    void recordRenderGraphTo(const CommandBufferPtr& commandBuffer);
    void recordCommandBuffer(
        const CommandBufferPtr& commandBuffer,
        const MaterialShaderNode& shaderNode);
    void bindShader(
        const MaterialShaderPtr& shader,
        const CommandBufferPtr& commandBuffer);
    static void recordCommandBuffer(
        const CommandBufferPtr& commandBuffer,
        const MaterialNode& materialNode,
        const MaterialShaderPtr& shader);
    static void bindMaterial(
        const MaterialPtr& material,
        const MaterialShaderPtr& shader,
        const CommandBufferPtr& commandBuffer);
    static void recordCommandBuffer(
        const CommandBufferPtr& commandBuffer,
        const MeshNode& meshNode,
        const MaterialShaderPtr& shader);
    static void bindObject(
        const MeshPtr& mesh,
        const MaterialShaderPtr& shader,
        const CommandBufferPtr& commandBuffer);

    void initGraphicsResources();
    BufferPtr createAndBindUniformBuffer(VkDeviceSize bufferSize);
    [[nodiscard]] VkPipelineLayout createDefaultPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    [[nodiscard]] VkPipeline createDefaultPipelineFor(const MaterialShaderPtr& shader, VkPipelineLayout pipelineLayout);
    void createRenderPass();

    void beginMainLoop() override;
    void lockMouseCursor(bool lock = true);
    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const PointLight& light);

    void update(float deltaTime) override;
    void updateCamera(float deltaTime);
    virtual void updateProjection() {};
    glm::mat4 calcDefaultProjection();
    void updateDevTools() override;

    void cleanup() override;
    void cleanupSwapChain() override;
    void recreateSwapChain() override;

    void initMaterialUniformBuffer(const MaterialPtr& material, const MaterialShaderPtr& shader);
    void initMaterialDescriptorSetLayout(const MaterialPtr& material, const MaterialShaderPtr& shader);
    VkDescriptorSet createMaterialDescriptorSetFor(const MaterialPtr& material, VkDescriptorSetLayout descriptorSetLayout);
    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorImageInfo* descriptorImageInfo);
    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorBufferInfo* descriptorBufferInfo);


    VkPipeline wireframePipeline = VK_NULL_HANDLE;
    bool wireframe = false;

    FlyCamera camera;

    glm::vec2 lastMousePos {};
    bool mouseCursorLocked = false;

    VkDescriptorSetLayout sceneDescriptorSetLayout_ = VK_NULL_HANDLE;
    VkDescriptorSet sceneDescriptorSet_ = VK_NULL_HANDLE;
    std::shared_ptr<Buffer> sceneDataBuffer_;
    SceneData sceneData_ {};

    VkDescriptorSetLayout meshDescriptorSetLayout_ = VK_NULL_HANDLE;

    RenderGraph renderGraph;

    PointLight light_ { "point" };
};

} // namespace rfx
