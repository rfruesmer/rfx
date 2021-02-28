#pragma once

#include "rfx/application/Application.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/FlyCamera.h"


namespace rfx {

class SceneTest : public Application
{
    struct UniformBufferObject {
        glm::mat4 proj;
        glm::mat4 view;
    };

public:
    SceneTest();

protected:
    void initGraphics() override;
    void beginMainLoop() override;
    void update(float deltaTime) override;
    void updateUniformBuffer();
    void updateDevTools() override;
    void cleanup() override;
    void cleanupSwapChain() override;
    void recreateSwapChain() override;

    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

private:
    void loadScene();
    void loadShaders();
    void initGraphicsResources();
    void createUniformBuffer();
    void createDescriptorPool();
    void createDescriptorSetLayouts();
    void createDescriptorSets();
    void createRenderPass();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawSceneNode(
        const std::shared_ptr<SceneNode>& sceneNode,
        const std::shared_ptr<CommandBuffer>& commandBuffer);
    void updateCamera(float deltaTime);
    void lockMouseCursor(bool lock = true);


    const VertexFormat vertexFormat { VertexFormat::COORDINATES | VertexFormat::COLORS_3 };

    VkPipeline wireframePipeline = VK_NULL_HANDLE;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<Buffer> uniformBuffer;
    UniformBufferObject ubo {};
    VkDescriptorSetLayout uniformBufferDSL = VK_NULL_HANDLE;
    VkDescriptorSet uniformBufferDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSetLayout imageSamplerDSL = VK_NULL_HANDLE;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
    bool wireframe = false;

    FlyCamera camera;
    glm::vec2 lastMousePos;
    bool mouseCursorLocked = false;
};

} // namespace rfx


