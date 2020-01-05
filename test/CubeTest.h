#pragma once

#include "rfx/application/Win32Application.h"

namespace rfx
{

class CubeTest : public Win32Application
{
public:
    static const int NUM_DESCRIPTOR_SETS = 1;
    static const uint64_t FENCE_TIMEOUT = 100000000;
    static const uint32_t NUM_VIEWPORTS = 1;
    static const uint32_t NUM_SCISSORS = NUM_VIEWPORTS;

    explicit CubeTest(HINSTANCE instanceHandle);

    void initialize() override;
    void update() override;
    void draw() override;

    void onResized(Window* window, int clientWidth, int clientHeight) override;

protected:
    void shutdown() override;

private:
    void initCamera();
    void initPipelineLayout();
    void initDescriptorSet();
    void initCommandPool();
    void initCommandBuffer();
    void initRenderPass();
    void initVertexShaderModule();
    void initFragmentShaderModule();
    void initFrameBuffers();
    void initVertexBuffer();
    void initPipeline();

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
    std::shared_ptr<CommandBuffer> commandBuffer;
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2] = {};
    std::shared_ptr<Buffer> uniformBuffer;
    std::shared_ptr<Buffer> vertexBuffer;
    VkVertexInputBindingDescription vertexInputBinding = {};
    VkVertexInputAttributeDescription vertexInputAttributes[2] = {};
    VkPipeline pipeline = nullptr;
};

} // namespace rfx

