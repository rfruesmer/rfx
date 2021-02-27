#pragma once

#include "rfx/application/Application.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class SceneTest : public Application
{
    struct UniformBufferObject {
        glm::mat4 proj;
        glm::mat4 view;
        glm::vec4 lightPos = glm::vec4(5.0f, 5.0f, -5.0f, 1.0f);
    };

public:
    SceneTest();

protected:
    void initGraphics() override;
    void update() override;
    void cleanup() override;
    void recreateSwapChain() override;

private:
    void loadScene();
    void loadShaders();
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


    std::shared_ptr<Scene> scene;
    std::shared_ptr<Buffer> uniformBuffer;
    UniformBufferObject ubo {};
    VkDescriptorSetLayout uniformBufferDSL = VK_NULL_HANDLE;
    VkDescriptorSet uniformBufferDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSetLayout imageSamplerDSL = VK_NULL_HANDLE;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;

    void initGraphicsResources();
};

} // namespace rfx


