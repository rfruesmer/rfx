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
    static const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;
    static const uint32_t NUM_VIEWPORTS = 1;
    static const uint32_t NUM_SCISSORS = NUM_VIEWPORTS;

    explicit TestApplication(std::filesystem::path configurationPath, handle_t instanceHandle);

    void updateModelViewProjection();
    void update() override;
    void draw() override;
    void destroyDescriptors();
    void destroyBuffers() const;
    void destroySwapChainAndDepthBuffer() const;
    void createSwapChainAndDepthBuffer() const;
    void destroyRenderPass();
    void destroyPipelineLayout();
    void destroyPipeline();

protected:
    void initCommandPool();
    void initRenderPass();
    void initFrameBuffers();

    virtual void initScene() = 0;
    void initCamera();

    virtual void initDescriptorSetLayout();
    void initPipelineLayout();
    virtual void initPipeline() = 0;
    void initDescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes);
    virtual void initDescriptorSet() = 0;
    virtual void initCommandBuffers() = 0;


    VkPipelineDynamicStateCreateInfo createDynamicState(uint32_t dynamicStateCount, VkDynamicState dynamicStates[]);
    VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState();
    VkPipelineRasterizationStateCreateInfo createRasterizationState();
    VkPipelineColorBlendAttachmentState createColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo createColorBlendState(
        const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState);
    VkPipelineViewportStateCreateInfo createViewportState();
    VkPipelineDepthStencilStateCreateInfo createDepthStencilState();
    VkPipelineMultisampleStateCreateInfo createMultiSampleState();


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

    VkPipelineLayout pipelineLayout = nullptr;
    VkDescriptorPool descriptorPool = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout;
    VkRenderPass renderPass = nullptr;
    std::vector<VkFramebuffer> frameBuffers;
    std::shared_ptr<CommandPool> commandPool;
    std::vector<std::shared_ptr<CommandBuffer>> drawCommandBuffers;
    std::shared_ptr<Buffer> uniformBuffer;
    VkPipeline pipeline = nullptr;
};

}
