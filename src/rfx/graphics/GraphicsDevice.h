#pragma once

#include "rfx/graphics/GraphicsDeviceInfo.h"
#include "rfx/graphics/Queue.h"
#include "rfx/graphics/CommandPool.h"
#include "rfx/graphics/Buffer.h"


namespace rfx
{

struct SwapChainBuffer {
    VkImage image;
    VkImageView view;
};

struct DepthBuffer {
    VkFormat format;
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
};

class GraphicsDevice
{
public:
    static const VkFormat DEPTHBUFFER_FORMAT = VK_FORMAT_D16_UNORM;

    explicit GraphicsDevice(VkDevice logicalDevice,
        VkPhysicalDevice vkPhysicalDevice,
        GraphicsDeviceInfo deviceInfo,
        PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr);

    ~GraphicsDevice();


    void initialize();
    void dispose();
    void resize(int width, int height) {}

    const GraphicsDeviceInfo& getDeviceInfo() const;
    const VkSwapchainKHR& getSwapChain() const;
    const std::vector<SwapChainBuffer>& getSwapChainBuffers() const;
    const DepthBuffer& getDepthBuffer() const;
    const std::shared_ptr<Queue>& getGraphicsQueue() const;
    const std::shared_ptr<Queue>& getPresentQueue() const;

    std::shared_ptr<CommandPool> createCommandPool(uint32_t queueFamilyIndex);
    void destroyCommandPool(const std::shared_ptr<CommandPool>& commandPool);

    std::shared_ptr<Buffer> createUniformBuffer(size_t size);
    std::shared_ptr<Buffer> createVertexBuffer(size_t size);
    std::shared_ptr<Buffer> createBuffer(size_t size, VkBufferUsageFlags usage);

    VkDescriptorPool createDescriptorPool(const VkDescriptorPoolCreateInfo& createInfo) const;
    void destroyDescriptorPool(VkDescriptorPool& inOutDescriptorPool) const;

    VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo) const;
    void destroyDescriptorSetLayout(VkDescriptorSetLayout& inOutDescriptorSetLayout) const;

    void allocateDescriptorSets(const VkDescriptorSetAllocateInfo& allocateInfo,
        std::vector<VkDescriptorSet>& outDescriptorSets) const;
    void updateDescriptorSets(size_t count, const VkWriteDescriptorSet* writes) const;

    VkPipelineLayout createPipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const;
    void destroyPipelineLayout(VkPipelineLayout& inOutPipelineLayout) const;

    VkSemaphore createSemaphore(const VkSemaphoreCreateInfo& createInfo) const;
    void destroySemaphore(VkSemaphore& inOutSemaphore) const;

    uint32_t acquireNextSwapChainImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence);
    VkFormat getSwapChainFormat() const;

    VkRenderPass createRenderPass(const VkRenderPassCreateInfo& createInfo) const;
    void destroyRenderPass(VkRenderPass& inOutRenderPass) const;

    VkShaderModule createShaderModule(const VkShaderModuleCreateInfo& createInfo) const;
    void destroyShaderModule(VkShaderModule& inOutShaderModule) const;

    VkFramebuffer createFrameBuffer(const VkFramebufferCreateInfo& createInfo) const;
    void destroyFrameBuffer(VkFramebuffer& inOutFrameBuffer) const;

    VkFence createFence(const VkFenceCreateInfo& createInfo) const;
    void destroyFence(VkFence& inOutFence) const;
    VkResult waitForFences(uint32_t count, const VkFence* fences, bool waitAll, uint64_t timeout) const;

    VkPipeline createGraphicsPipeline(const VkGraphicsPipelineCreateInfo& createInfo) const;
    void destroyPipeline(VkPipeline& inOutPipeline) const;

private:
    void disposeDevice();
    void disposeSwapChain();
    void disposeDepthBuffer();
    void disposeCommandPools();

    void loadDeviceFunctions();

    void createDefaultQueues();
    std::shared_ptr<Queue> createQueue(uint32_t queueFamilyIndex) const;

    void createSwapChain();
    void getSwapChainSurfaceFormat(VkFormat& inOutFormat, VkColorSpaceKHR& inOutColorSpace) const;
    VkSurfaceTransformFlagBitsKHR getSwapChainTransform() const;
    VkCompositeAlphaFlagBitsKHR getSwapChainCompositeAlpha() const;

    void createDepthBuffer();

    VulkanDeviceFunctionPtrs vk = {};
    VkDevice vkDevice = nullptr;
    VkPhysicalDevice vkPhysicalDevice = nullptr;
    GraphicsDeviceInfo deviceInfo;
 
    VkSwapchainKHR swapChain = nullptr;
    VkFormat swapChainFormat = VK_FORMAT_UNDEFINED;
    std::vector<SwapChainBuffer> swapChainBuffers;

    DepthBuffer depthBuffer = {};

    std::vector<QueueFamilyInfo> queueFamilies;
    std::shared_ptr<Queue> graphicsQueue = nullptr;
    std::shared_ptr<Queue> presentQueue = nullptr;
    std::unordered_set<std::shared_ptr<CommandPool>> commandPools;

    DECLARE_VULKAN_FUNCTION(vkGetDeviceProcAddr);
    DECLARE_VULKAN_FUNCTION(vkGetDeviceQueue);
    DECLARE_VULKAN_FUNCTION(vkDeviceWaitIdle);
    DECLARE_VULKAN_FUNCTION(vkDestroyDevice);

    DECLARE_VULKAN_FUNCTION(vkCreateSemaphore);
    DECLARE_VULKAN_FUNCTION(vkDestroySemaphore);

    DECLARE_VULKAN_FUNCTION(vkCreateFence);
    DECLARE_VULKAN_FUNCTION(vkWaitForFences);
    DECLARE_VULKAN_FUNCTION(vkDestroyFence);

    DECLARE_VULKAN_FUNCTION(vkQueueSubmit);
    DECLARE_VULKAN_FUNCTION(vkQueuePresentKHR);

    DECLARE_VULKAN_FUNCTION(vkCreateRenderPass);
    DECLARE_VULKAN_FUNCTION(vkDestroyRenderPass);

    DECLARE_VULKAN_FUNCTION(vkCreateCommandPool);
    DECLARE_VULKAN_FUNCTION(vkDestroyCommandPool);
    DECLARE_VULKAN_FUNCTION(vkAllocateCommandBuffers);
    DECLARE_VULKAN_FUNCTION(vkBeginCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkEndCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkResetCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkFreeCommandBuffers);
    DECLARE_VULKAN_FUNCTION(vkCmdBeginRenderPass);
    DECLARE_VULKAN_FUNCTION(vkCmdEndRenderPass);
    DECLARE_VULKAN_FUNCTION(vkCmdBindVertexBuffers);
    DECLARE_VULKAN_FUNCTION(vkCmdBindPipeline);
    DECLARE_VULKAN_FUNCTION(vkCmdBindDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkCmdSetViewport);
    DECLARE_VULKAN_FUNCTION(vkCmdSetScissor);
    DECLARE_VULKAN_FUNCTION(vkCmdDraw);

    DECLARE_VULKAN_FUNCTION(vkGetImageMemoryRequirements);
    DECLARE_VULKAN_FUNCTION(vkBindImageMemory);
    DECLARE_VULKAN_FUNCTION(vkCreateImage);
    DECLARE_VULKAN_FUNCTION(vkDestroyImage);
    DECLARE_VULKAN_FUNCTION(vkCreateImageView);
    DECLARE_VULKAN_FUNCTION(vkDestroyImageView);

    DECLARE_VULKAN_FUNCTION(vkAllocateMemory);
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);
    DECLARE_VULKAN_FUNCTION(vkCreateBuffer);
    DECLARE_VULKAN_FUNCTION(vkDestroyBuffer);
    DECLARE_VULKAN_FUNCTION(vkGetBufferMemoryRequirements);
    DECLARE_VULKAN_FUNCTION(vkMapMemory);
    DECLARE_VULKAN_FUNCTION(vkUnmapMemory);
    DECLARE_VULKAN_FUNCTION(vkBindBufferMemory);

    DECLARE_VULKAN_FUNCTION(vkCreateDescriptorSetLayout);
    DECLARE_VULKAN_FUNCTION(vkDestroyDescriptorSetLayout);
    DECLARE_VULKAN_FUNCTION(vkCreateDescriptorPool);
    DECLARE_VULKAN_FUNCTION(vkAllocateDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkUpdateDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkDestroyDescriptorPool);
    DECLARE_VULKAN_FUNCTION(vkCreatePipelineLayout);
    DECLARE_VULKAN_FUNCTION(vkDestroyPipelineLayout);

    DECLARE_VULKAN_FUNCTION(vkCreateShaderModule);
    DECLARE_VULKAN_FUNCTION(vkDestroyShaderModule);

    DECLARE_VULKAN_FUNCTION(vkCreateFramebuffer);
    DECLARE_VULKAN_FUNCTION(vkDestroyFramebuffer);

    DECLARE_VULKAN_FUNCTION(vkCreateGraphicsPipelines);
    DECLARE_VULKAN_FUNCTION(vkDestroyPipeline);

    DECLARE_VULKAN_FUNCTION(vkCreateSwapchainKHR);
    DECLARE_VULKAN_FUNCTION(vkGetSwapchainImagesKHR);
    DECLARE_VULKAN_FUNCTION(vkAcquireNextImageKHR);
    DECLARE_VULKAN_FUNCTION(vkDestroySwapchainKHR);
};

} // namespace rfx
