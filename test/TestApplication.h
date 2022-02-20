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
        glm::mat4 viewProjMatrix;
        glm::vec3 cameraPosition;
        float padding;
    };

    struct MeshData {
        [[maybe_unused]] glm::mat4 modelMatrix;
    };

    void initGraphics() override;

    void createDescriptorPool();

    virtual void createSceneResources();
    void destroySceneResources();
    void createSceneDescriptorSetLayout();
    void createSceneDescriptorSet();
    void createSceneDataBuffer();
    void updateSceneData(float deltaTime);
    void updateSceneDataBuffer();

    void createShadersFor(
        const ScenePtr& scene,
        const std::string& defaultShaderId);
    void createShadersFor(
        const ModelPtr& model,
        MaterialShaderFactory& shaderFactory);
    virtual void initShaderFactory(MaterialShaderFactory& shaderFactory) = 0;

    virtual void createMeshResources();
    void destroyMeshResources();
    void createMeshDescriptorSetLayout();
    void createMeshDescriptorSets(const ScenePtr& scene);
    void createMeshDescriptorSets(const ModelPtr& model);
    void createMeshDataBuffers(const ScenePtr& scene);
    void createMeshDataBuffers(const ModelPtr& model);

    virtual void createPipelines();
    virtual void buildRenderGraph() {}
    void destroyRenderGraph();
    virtual void createCommandBuffers();

    void initGraphicsResources();
    BufferPtr createAndBindUniformBuffer(VkDeviceSize bufferSize);
    [[nodiscard]] VkPipeline createPipelineFor(
        const ShaderProgramPtr& shaderProgram,
        VkPipelineLayout pipelineLayout);
    void createRenderPass();

    void beginMainLoop() override;
    void lockMouseCursor(bool lock = true);
    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

    virtual void updateSceneData();

    void update(float deltaTime) override;
    void updateCamera(float deltaTime);
    void updateProjection();
    glm::mat4 calcDefaultProjection();
    virtual void updateShaderData() {};
    void updateDevTools() override;

    void cleanup() override;
    void destroyShaderMap();
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

    std::shared_ptr<FlyCamera> camera = std::make_shared<FlyCamera>();

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
