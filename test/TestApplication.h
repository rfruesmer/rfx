#pragma once

#include <rfx/scene/PointLight.h>
#include "rfx/application/Application.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/FlyCamera.h"
#include "rfx/scene/MaterialShader.h"


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

    virtual void createEffects() = 0;

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

    virtual void createMaterialResources() = 0;

    virtual void createPipelineLayouts() = 0;
    virtual void createPipelines() = 0;
    virtual void createCommandBuffers() = 0;

    void initGraphicsResources();
    std::shared_ptr<Buffer> createAndBindUniformBuffer(VkDeviceSize size, const void* data);
    void createDefaultPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
    void createDefaultPipeline(const MaterialShader& effect);
    void createRenderPass();

    void beginMainLoop() override;
    void lockMouseCursor(bool lock = true);
    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

    static VkWriteDescriptorSet buildWriteDescriptorSet(
        VkDescriptorSet descriptorSet,
        uint32_t binding,
        const VkDescriptorBufferInfo* descriptorBufferInfo);

    void setProjectionMatrix(const glm::mat4& projection);
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setLight(const PointLight& light);


    void update(float deltaTime) override;
    void updateCamera(float deltaTime);
    virtual void updateProjection() {};
    glm::mat4 calcDefaultProjection();
    void updateDevTools() override;

    void cleanupSwapChain() override;
    void recreateSwapChain() override;


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

    PointLight light_ { "point" };
};

} // namespace rfx
