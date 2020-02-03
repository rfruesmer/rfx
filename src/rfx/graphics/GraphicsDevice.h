#pragma once

#include "rfx/graphics/GraphicsDeviceInfo.h"
#include "rfx/graphics/command/Queue.h"
#include "rfx/graphics/command/CommandPool.h"
#include "rfx/graphics/buffer/Buffer.h"
#include "rfx/graphics/buffer/VertexBuffer.h"
#include "rfx/graphics/buffer/IndexBuffer.h"
#include "rfx/graphics/shader/VertexShader.h"
#include "rfx/graphics/shader/FragmentShader.h"
#include "rfx/graphics/texture/Texture2D.h"
#include "rfx/graphics/texture/Image.h"
#include "rfx/graphics/window/Window.h"


namespace rfx
{

struct SwapChainBuffer {
    VkImage image;
    VkImageView imageView;
};

struct SwapChainProperties
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
    uint32_t imageCount = 0;
    VkExtent2D imageSize = {};
};

struct DepthBuffer {
    VkFormat format;
    VkImage image;
    VkImageView imageView;
    VkDeviceMemory deviceMemory;
};

class GraphicsDevice
{
public:
    static const VkFormat DEPTHBUFFER_FORMAT = VK_FORMAT_D16_UNORM;
    static const VkFormat DEFAULT_SWAPCHAIN_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;
    static const VkColorSpaceKHR DEFAULT_COLORSPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    explicit GraphicsDevice(VkDevice vkLogicalDevice,
        VkPhysicalDevice vkPhysicalDevice,
        VkSurfaceKHR presentationSurface,
        GraphicsDeviceInfo deviceInfo,
        const std::shared_ptr<Window>& window,
        PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr,
        PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
        PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR,
        PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties);

    ~GraphicsDevice();

    void initialize();
    void dispose();
    void waitIdle() const;

    void createDepthBuffer();
    void destroyDepthBuffer();

    void createSwapChain();
    void createSwapChain(VkFormat desiredFormat, VkColorSpaceKHR desiredColorSpace);
    void destroySwapChain();

    std::shared_ptr<CommandPool> createCommandPool(uint32_t queueFamilyIndex);
    void destroyCommandPool(const std::shared_ptr<CommandPool>& commandPool);

    std::shared_ptr<Buffer> createUniformBuffer(size_t size);
    std::shared_ptr<VertexBuffer> createVertexBuffer(uint32_t vertexCount, const VertexFormat& vertexFormat);
    std::shared_ptr<IndexBuffer> createIndexBuffer(uint32_t indexCount, VkIndexType indexFormat);
    std::shared_ptr<Buffer> createBuffer(size_t size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties) const;

    std::shared_ptr<VertexShader>  createVertexShader(
        const VkPipelineShaderStageCreateInfo& createInfo, 
        const VertexFormat& vertexFormat);
    std::shared_ptr<FragmentShader> createFragmentShader(
        const VkPipelineShaderStageCreateInfo& createInfo);

    std::shared_ptr<Texture2D> createTexture2D(int width, int height,
        VkFormat format, const std::vector<std::byte>& data);

    VkDescriptorPool createDescriptorPool(const VkDescriptorPoolCreateInfo& createInfo) const;
    void destroyDescriptorPool(VkDescriptorPool& inOutDescriptorPool) const;

    VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo) const;
    void destroyDescriptorSetLayout(VkDescriptorSetLayout& inOutDescriptorSetLayout) const;

    void allocateDescriptorSets(const VkDescriptorSetAllocateInfo& allocateInfo,
        std::vector<VkDescriptorSet>& outDescriptorSets) const;
    void updateDescriptorSets(const std::vector<VkWriteDescriptorSet>& writes) const;

    VkPipelineLayout createPipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const;
    void destroyPipelineLayout(VkPipelineLayout& inOutPipelineLayout) const;

    VkSemaphore createSemaphore(const VkSemaphoreCreateInfo& createInfo) const;
    void destroySemaphore(VkSemaphore& inOutSemaphore) const;

    VkResult acquireNextSwapChainImage(uint64_t timeout, VkSemaphore semaphore, 
        VkFence fence, uint32_t& outImageIndex) const;
    const SwapChainProperties& getSwapChainProperties() const;
    VkFormat getSwapChainFormat() const;

    VkRenderPass createRenderPass(const VkRenderPassCreateInfo& createInfo) const;
    void destroyRenderPass(VkRenderPass& inOutRenderPass) const;

    VkShaderModule createShaderModule(const VkShaderModuleCreateInfo& createInfo) const;
    void destroyShaderModule(VkShaderModule& inOutShaderModule) const;

    VkFramebuffer createFrameBuffer(const VkFramebufferCreateInfo& createInfo) const;
    void destroyFrameBuffer(VkFramebuffer& inOutFrameBuffer) const;

    VkFence createFence() const;
    VkFence createFence(const VkFenceCreateInfo& createInfo) const;
    void destroyFence(VkFence& inOutFence) const;
    VkResult waitForFence(VkFence fence, uint64_t timeout) const;
    VkResult waitForFences(uint32_t count, const VkFence* fences, bool waitAll, uint64_t timeout) const;

    VkPipeline createGraphicsPipeline(const VkGraphicsPipelineCreateInfo& createInfo) const;
    void destroyPipeline(VkPipeline& inOutPipeline) const;

    const GraphicsDeviceInfo& getDeviceInfo() const;
    const VkSwapchainKHR& getSwapChain() const;
    const std::vector<SwapChainBuffer>& getSwapChainBuffers() const;
    const DepthBuffer& getDepthBuffer() const;
    const std::shared_ptr<Queue>& getGraphicsQueue() const;
    const std::shared_ptr<Queue>& getPresentQueue() const;
    const std::shared_ptr<CommandPool>& getTempCommandPool() const;

private:
    void destroyDevice();
    void destroyCommandPools();

    void loadDeviceFunctions();
    void createSingleTimeCommandPool();
    uint32_t findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask) const;

    void createDefaultQueues();
    std::shared_ptr<Queue> createQueue(uint32_t queueFamilyIndex) const;

    void createBufferInternal(size_t size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& outBuffer,
        VkDeviceMemory& outDeviceMemory,
        VkDeviceSize& outDeviceSize) const;

    std::shared_ptr<Image> createImage(uint32_t width, uint32_t height, VkFormat format, 
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties) const;
    void updateImage(const std::shared_ptr<Image>& image, 
        const std::vector<std::byte>& imageData) const;
    VkImageView createImageView(const std::shared_ptr<Image>&  image, VkFormat format) const;

    std::shared_ptr<CommandBuffer> beginSingleTimeCommands() const;
    void endSingleTimeCommands(const std::shared_ptr<CommandBuffer>& commandBuffer) const;

    VkSampler createTextureSampler() const;

    void querySwapChainProperties();
    void querySwapChainSurfaceCapabilities();
    void querySwapChainSurfaceFormats();
    void querySwapChainImageSize();
    void getSwapChainSurfaceFormat(VkFormat& inOutFormat, VkColorSpaceKHR& inOutColorSpace) const;
    VkSurfaceTransformFlagBitsKHR getSwapChainTransform() const;
    VkCompositeAlphaFlagBitsKHR getSwapChainCompositeAlpha() const;

    VulkanDeviceFunctionPtrs vk = {};
    VkDevice vkDevice = nullptr;
    VkPhysicalDevice vkPhysicalDevice = nullptr;
    VkSurfaceKHR presentationSurface = nullptr;
    GraphicsDeviceInfo deviceInfo = {};
    std::shared_ptr<Window> window;

    VkSwapchainKHR swapChain = nullptr;
    SwapChainProperties swapChainProperties = {};
    VkFormat swapChainFormat = VK_FORMAT_UNDEFINED;
    std::vector<SwapChainBuffer> swapChainBuffers;

    DepthBuffer depthBuffer = {};

    std::vector<QueueFamilyInfo> queueFamilies;
    std::shared_ptr<Queue> graphicsQueue = nullptr;
    std::shared_ptr<Queue> presentQueue = nullptr;
    std::unordered_set<std::shared_ptr<CommandPool>> commandPools;
    std::shared_ptr<CommandPool> tempCommandPool;

    DECLARE_VULKAN_FUNCTION(vkGetDeviceProcAddr);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
    DECLARE_VULKAN_FUNCTION(vkGetPhysicalDeviceFormatProperties);

    DECLARE_VULKAN_FUNCTION(vkDeviceWaitIdle);
    DECLARE_VULKAN_FUNCTION(vkDestroyDevice);

    DECLARE_VULKAN_FUNCTION(vkCreateSemaphore);
    DECLARE_VULKAN_FUNCTION(vkDestroySemaphore);

    DECLARE_VULKAN_FUNCTION(vkCreateFence);
    DECLARE_VULKAN_FUNCTION(vkWaitForFences);
    DECLARE_VULKAN_FUNCTION(vkDestroyFence);

    DECLARE_VULKAN_FUNCTION(vkGetDeviceQueue);
    DECLARE_VULKAN_FUNCTION(vkQueueSubmit);
    DECLARE_VULKAN_FUNCTION(vkQueueWaitIdle);
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
    DECLARE_VULKAN_FUNCTION(vkCmdBindIndexBuffer);
    DECLARE_VULKAN_FUNCTION(vkCmdBindPipeline);
    DECLARE_VULKAN_FUNCTION(vkCmdBindDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkCmdSetViewport);
    DECLARE_VULKAN_FUNCTION(vkCmdSetScissor);
    DECLARE_VULKAN_FUNCTION(vkCmdDraw);
    DECLARE_VULKAN_FUNCTION(vkCmdDrawIndexed);
    DECLARE_VULKAN_FUNCTION(vkCmdPipelineBarrier);
    DECLARE_VULKAN_FUNCTION(vkCmdCopyBuffer);
    DECLARE_VULKAN_FUNCTION(vkCmdCopyBufferToImage);

    DECLARE_VULKAN_FUNCTION(vkGetImageMemoryRequirements);
    DECLARE_VULKAN_FUNCTION(vkBindImageMemory);
    DECLARE_VULKAN_FUNCTION(vkCreateImage);
    DECLARE_VULKAN_FUNCTION(vkDestroyImage);
    DECLARE_VULKAN_FUNCTION(vkGetImageSubresourceLayout);
    DECLARE_VULKAN_FUNCTION(vkCreateImageView);
    DECLARE_VULKAN_FUNCTION(vkDestroyImageView);

    DECLARE_VULKAN_FUNCTION(vkCreateSampler);
    DECLARE_VULKAN_FUNCTION(vkDestroySampler);

    DECLARE_VULKAN_FUNCTION(vkAllocateMemory);
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);
    DECLARE_VULKAN_FUNCTION(vkCreateBuffer);
    DECLARE_VULKAN_FUNCTION(vkDestroyBuffer);
    DECLARE_VULKAN_FUNCTION(vkGetBufferMemoryRequirements);
    DECLARE_VULKAN_FUNCTION(vkMapMemory);
    DECLARE_VULKAN_FUNCTION(vkUnmapMemory);
    DECLARE_VULKAN_FUNCTION(vkBindBufferMemory);
    DECLARE_VULKAN_FUNCTION(vkInvalidateMappedMemoryRanges);

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
