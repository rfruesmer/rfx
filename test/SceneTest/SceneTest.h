#pragma once

#include "rfx/application/Application.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/FlyCamera.h"


namespace rfx {

class SceneTest : public Application
{
    struct UniformBufferObject {
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;
        glm::vec4 lightPos;          // light position in eye coords
    };

    struct MeshUniformBufferObject {
        glm::mat4 modelMatrix;
    };

    struct PushConstant {
        glm::vec4 Ld;                // diffuse light intensity
        glm::vec4 Kd;                // diffuse reflectivity (material property)
    };

    const VertexFormat vertexFormat {
        VertexFormat::COORDINATES | VertexFormat::NORMALS
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
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSetLayouts();
    void createDescriptorSets();
    void createRenderPass();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);
    void updateCamera(float deltaTime);
    void lockMouseCursor(bool lock = true);


    VkPipeline wireframePipeline = VK_NULL_HANDLE;
    bool wireframe = false;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;

    FlyCamera camera;
    glm::vec2 lastMousePos;
    bool mouseCursorLocked = false;

    std::shared_ptr<Buffer> sceneUniformBuffer;
    UniformBufferObject sceneUBO {};

    VkDescriptorSetLayout sceneDescSetLayout;
    VkDescriptorSet sceneDescSet {};

    VkDescriptorSetLayout meshDescSetLayout;
    std::vector<VkDescriptorSet> meshDescSets;
    std::vector<std::shared_ptr<Buffer>> meshUniformBuffers; // TODO: refactor to sub-buffers
};

} // namespace rfx


