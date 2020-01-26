#pragma once

#include "rfx/application/Win32Application.h"
#include "rfx/scene/Scene.h"
#include "rfx/graphics/effect/Effect.h"
#include "rfx/graphics/effect/EffectDefinition.h"
#include "rfx/scene/Camera.h"


namespace rfx
{
    struct ModelDefinition;


class TestApplication :
#ifdef _WINDOWS
    public Win32Application
#else
    static_assert(false, "not implemented yet");
#endif // _WINDOWS
{
public:
    static const int NUM_DESCRIPTOR_SETS = 1;
    static const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;
    static const uint32_t NUM_VIEWPORTS = 1;
    static const uint32_t NUM_SCISSORS = NUM_VIEWPORTS;

    explicit TestApplication(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void update() override;
    void draw() override;
    void destroyDescriptors();
    void destroyBuffers() const;
    void destroySwapChainAndDepthBuffer() const;
    void createSwapChainAndDepthBuffer() const;
    void destroyRenderPass();

protected:
    void initCommandPool();
    void initRenderPass();
    void initFrameBuffers();

    virtual void initScene();
    void loadEffectsDefaults();
    void loadEffectDefaults(const Json::Value& jsonEffectDefaults);
    void loadScene();
    void initCamera();

    virtual void initCommandBuffers();

    void recreateSwapChain();
    void destroyFrameBuffers();
    void freeCommandBuffers() const;

    void onViewProjectionMatrixUpdated();

    void shutdown() override;

    VkDescriptorSetLayout descriptorSetLayout = nullptr;

    VkRenderPass renderPass = nullptr;
    std::vector<VkFramebuffer> frameBuffers;
    std::shared_ptr<CommandPool> commandPool;
    std::vector<std::shared_ptr<CommandBuffer>> drawCommandBuffers;
    std::unique_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::unordered_map<std::string, EffectDefinition> effectDefaults;
    std::vector<std::shared_ptr<Effect>> effects;

private:
    void updateViewProjectionMatrix();
    void drawSceneNode(const std::unique_ptr<SceneNode>& sceneNode,
        const std::shared_ptr<CommandBuffer>& commandBuffer);
};

}
