#pragma once

#include "rfx/application/Application.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/FlyCamera.h"
#include "effects/VertexDiffuseEffect.h"


namespace rfx {

class SceneTest : public Application
{


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

    std::unique_ptr<VertexDiffuseEffect> effect;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;

    FlyCamera camera;
    glm::vec2 lastMousePos;
    bool mouseCursorLocked = false;



};

} // namespace rfx


