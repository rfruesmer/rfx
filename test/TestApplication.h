#pragma once

#include "rfx/application/Win32Application.h"

namespace rfx
{


class TestApplication :
#ifdef _WINDOWS
    public Win32Application
#else
    static_assert(false, "not implemented yet");
#endif // _WINDOWS
{
public:
    static const int NUM_DESCRIPTOR_SETS = 1;
    static const uint64_t FENCE_TIMEOUT = 100000000;
    static const uint32_t NUM_VIEWPORTS = 1;
    static const uint32_t NUM_SCISSORS = NUM_VIEWPORTS;

    virtual uint32_t getVertexSize() const = 0;
    virtual uint32_t getVertexCount() const = 0;
    virtual const std::byte* getVertexData() const = 0;

    virtual std::string getVertexShaderPath() const = 0;
    virtual std::string getFragmentShaderPath() const = 0;

    explicit TestApplication(HINSTANCE instanceHandle);

    void initialize() override;
    void updateModelViewProjection();
    void update() override;
    void draw() override;
    void destroyShaderModules();
    void destroyDescriptors();
    void destroyBuffers() const;
    void destroySwapChainAndDepthBuffer() const;
    void createSwapChainAndDepthBuffer() const;
    void destroyRenderPass();
    void destroyPipelineLayout();
    void destroyPipeline();

protected:
    void initCamera();
    virtual void initPipelineLayout() = 0;
    virtual void initDescriptorPool() = 0;
    virtual void initDescriptorSet() = 0;
    void initCommandPool();
    void initCommandBuffers();
    void initRenderPass();
    void initVertexShaderModule();
    void initFragmentShaderModule();
    void initFrameBuffers();
    virtual void initVertexBuffer();
    void initPipeline();

    void recreateSwapChain();
    void destroyFrameBuffers();
    void freeCommandBuffers() const;

    void shutdown() override;

    glm::vec3 cameraPosition;
    glm::vec3 cameraLookAt;
    glm::vec3 cameraUp;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelViewProjMatrix;
    glm::mat4 clipMatrix;

    VkPipelineLayout pipelineLayout = nullptr;
    VkDescriptorPool descriptorPool = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout;
    VkRenderPass renderPass = nullptr;
    std::vector<VkFramebuffer> frameBuffers;
    std::shared_ptr<CommandPool> commandPool;
    std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2] = {};
    std::shared_ptr<Buffer> uniformBuffer;
    std::shared_ptr<Buffer> vertexBuffer;
    VkVertexInputBindingDescription vertexInputBinding = {};
    VkVertexInputAttributeDescription vertexInputAttributes[2] = {};
    VkPipeline pipeline = nullptr;
};

}
