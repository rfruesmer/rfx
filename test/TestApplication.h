#pragma once

#include "rfx/application/Application.h"
#include "rfx/rendering/RenderGraph.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/FlyCamera.h"
#include "rfx/scene/MaterialShaderFactory.h"


namespace rfx {

class TestApplication : public Application
{
protected:
    struct SceneData {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
    };

    struct MeshData {
        [[maybe_unused]] glm::mat4 modelMatrix;
    };

    void createDescriptorPool();

    void createSceneResources();
    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createSceneDataBuffer();
    void updateSceneData(float deltaTime);
    void updateSceneDataBuffer();

    void createShadersFor(
        const ModelPtr& model,
        const std::string& defaultShaderId);
    virtual void initShaderFactory(MaterialShaderFactory& shaderFactory) = 0;

    virtual void createMeshResources();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets(const ModelPtr& model);
    void createMeshDataBuffers(const ModelPtr& model);

    virtual void createPipelines();
    virtual void createCommandBuffers();

    void initGraphicsResources();
    BufferPtr createAndBindUniformBuffer(VkDeviceSize bufferSize);
    [[nodiscard]] VkPipelineLayout createDefaultPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    [[nodiscard]] VkPipeline createDefaultPipelineFor(const MaterialShaderPtr& shader, VkPipelineLayout pipelineLayout);
    void createRenderPass();

    void beginMainLoop() override;
    void lockMouseCursor(bool lock = true);
    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

    void setProjectionMatrix(const glm::mat4& projection);
    virtual void setViewMatrix(const glm::mat4& viewMatrix);

    void update(float deltaTime) override;
    void updateCamera(float deltaTime);
    virtual void updateProjection();
    glm::mat4 calcDefaultProjection();
    virtual void updateShaderData() {};
    void updateDevTools() override;

    void cleanup() override;
    void cleanupSwapChain() override;
    void recreateSwapChain() override;

    void initMaterialUniformBuffer(const MaterialPtr& material, const MaterialShaderPtr& shader);
    void initMaterialDescriptorSet(const MaterialPtr& material, const MaterialShaderPtr& shader);
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

    std::unordered_map<MaterialShaderPtr, std::vector<MaterialPtr>> materialShaderMap;

    RenderGraphPtr renderGraph;
};

} // namespace rfx
