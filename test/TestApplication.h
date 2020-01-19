#pragma once

#include "rfx/application/Win32Application.h"
#include "rfx/scene/SceneNode.h"
#include "rfx/scene/ModelDefinitionDeserializer.h"
#include "rfx/graphics/effect/Effect.h"
#include "rfx/graphics/effect/EffectDefinition.h"


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
    void createSceneGraphRootNode();
    void loadModels();
    ModelDefinition deserialize(const Json::Value& jsonModelDefinition,
        const ModelDefinitionDeserializer& deserializer) const;
    std::shared_ptr<Mesh> loadModel(const ModelDefinition& modelDefinition,
        const std::shared_ptr<Effect>& effect) const;
    void attachToSceneGraph(const std::shared_ptr<Mesh>& mesh, const ModelDefinition& modelDefinition) const;
    void initCamera();

    virtual void initCommandBuffers();

    void recreateSwapChain();
    void destroyFrameBuffers();
    void freeCommandBuffers() const;

    void onViewProjectionMatrixUpdated();

    void shutdown() override;

    glm::vec3 cameraPosition;
    glm::vec3 cameraLookAt;
    glm::vec3 cameraUp;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjMatrix;

    VkDescriptorSetLayout descriptorSetLayout = nullptr;

    VkRenderPass renderPass = nullptr;
    std::vector<VkFramebuffer> frameBuffers;
    std::shared_ptr<CommandPool> commandPool;
    std::vector<std::shared_ptr<CommandBuffer>> drawCommandBuffers;
    std::unique_ptr<SceneNode> sceneGraph;
    std::unordered_map<std::string, EffectDefinition> effectDefaults;
    std::vector<std::shared_ptr<Effect>> effects;

private:
    std::shared_ptr<Effect> loadEffect(const EffectDefinition& effectDefinition);
    std::shared_ptr<Effect> createEffect(const EffectDefinition& effectDefinition,
        std::unique_ptr<ShaderProgram>& shaderProgram,
        const std::vector<std::shared_ptr<Texture2D>>& textures);
    void updateViewProjectionMatrix();
    void drawSceneNode(const std::unique_ptr<SceneNode>& sceneNode,
        const std::shared_ptr<CommandBuffer>& commandBuffer);
};

}
