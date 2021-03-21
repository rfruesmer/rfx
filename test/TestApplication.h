#pragma once

#include "rfx/application/Application.h"
#include "rfx/scene/Model.h"
#include "rfx/scene/FlyCamera.h"
#include "rfx/scene/Effect.h"


namespace rfx {

class TestApplication : public Application
{
protected:
    virtual void createEffects() = 0;
    virtual void createUniformBuffers() = 0;
    virtual void createDescriptorPools() = 0;
    virtual void createDescriptorSetLayouts() = 0;
    virtual void createDescriptorSets() = 0;
    virtual void createPipelineLayouts() = 0;
    virtual void createPipelines() = 0;
    virtual void createCommandBuffers() = 0;

    void initGraphicsResources();
    void createDefaultPipelineLayout(const Effect& effect);
    void createDefaultPipeline(const Effect& effect);
    void createRenderPass();

    void beginMainLoop() override;
    void lockMouseCursor(bool lock = true);
    void onKeyEvent(const Window& window, int key, int scancode, int action, int mods) override;

    void update(float deltaTime) override;
    virtual void updateSceneData(float deltaTime) = 0;
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
};

} // namespace rfx
