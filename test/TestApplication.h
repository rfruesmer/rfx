#pragma once

#include "rfx/application/Application.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/FlyCamera.h"
#include "rfx/scene/Effect.h"


namespace rfx {

class TestApplication : public Application
{
protected:
    void loadShaders();

    void initGraphicsResources();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSetLayouts();
    void createDescriptorSets();
    void createRenderPass();
    void createPipelineLayout();
    void createPipeline();
    virtual void createCommandBuffers() = 0;

    void beginMainLoop() override;
    void lockMouseCursor(bool lock = true);
    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

    void update(float deltaTime) override;
    virtual void updateSceneData() = 0;
    void updateCamera(float deltaTime);
    virtual void updateProjection() {};
    glm::mat4 calcDefaultProjection();
    void updateDevTools() override;

    void cleanup() override;
    void cleanupSwapChain() override;
    void recreateSwapChain() override;


    VkPipeline wireframePipeline = VK_NULL_HANDLE;
    bool wireframe = false;

    std::shared_ptr<Scene> scene;
    FlyCamera camera;
    std::unique_ptr<Effect> effect;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;

    glm::vec2 lastMousePos {};
    bool mouseCursorLocked = false;
};

} // namespace rfx
