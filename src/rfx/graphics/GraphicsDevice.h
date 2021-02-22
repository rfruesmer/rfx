#pragma once

#include "rfx/graphics/GraphicsDeviceDesc.h"
#include "rfx/graphics/SwapChain.h"
#include "rfx/graphics/DepthBuffer.h"
#include "rfx/graphics/Queue.h"
#include "rfx/graphics/CommandBuffer.h"
#include "rfx/graphics/VertexBuffer.h"
#include "rfx/graphics/VertexFormat.h"
#include "rfx/graphics/IndexBuffer.h"
#include "rfx/graphics/Texture2D.h"
#include "rfx/graphics/Image.h"
#include "rfx/graphics/ImageDesc.h"


namespace rfx {

class GraphicsDevice
{
public:
    static const VkFormat DEFAULT_SWAPCHAIN_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;
    static const VkColorSpaceKHR DEFAULT_COLORSPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    static const VkFormat DEFAULT_DEPTHBUFFER_FORMAT = VK_FORMAT_D16_UNORM;
    static const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000; // in nanoseconds

    GraphicsDevice(
        GraphicsDeviceDesc desc,
        VkPhysicalDevice physicalDevice,
        VkDevice logicalDevice,
        std::shared_ptr<Queue> graphicsQueue,
        std::shared_ptr<Queue> presentQueue,
        VkSurfaceKHR presentSurface);

    ~GraphicsDevice();

    const GraphicsDeviceDesc& getDesc() const;

    void createSwapChain(
        uint32_t width,
        uint32_t height);

    void createSwapChain(
        uint32_t width,
        uint32_t height,
        VkFormat desiredFormat,
        VkColorSpaceKHR desiredColorSpace);

    [[nodiscard]]
    const std::unique_ptr<SwapChain>& getSwapChain() const;

    void createDepthBuffer(VkFormat format);

    [[nodiscard]]
    const std::unique_ptr<DepthBuffer>& getDepthBuffer() const;

    [[nodiscard]]
    std::shared_ptr<Buffer> createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags memoryProperties) const;

    [[nodiscard]]
    std::shared_ptr<VertexBuffer> createVertexBuffer(uint32_t vertexCount, const VertexFormat& vertexFormat) const;

    [[nodiscard]]
    std::shared_ptr<IndexBuffer> createIndexBuffer(uint32_t indexCount, VkIndexType indexType);

    [[nodiscard]]
    std::shared_ptr<CommandBuffer> createCommandBuffer(VkCommandPool commandPool) const;
    void flush(const std::shared_ptr<CommandBuffer>& commandBuffer) const;
    void destroyCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer, VkCommandPool commandPool) const;

    [[nodiscard]]
    std::vector<std::shared_ptr<CommandBuffer>> createCommandBuffers(VkCommandPool commandPool, uint32_t count) const;

    [[nodiscard]]
    std::shared_ptr<Texture2D> createTexture2D(
        const ImageDesc& imageDesc,
        const std::vector<std::byte>& imageData,
        bool isGenerateMipmaps) const;

    [[nodiscard]]
    std::shared_ptr<Image> createImage(
        VkFormat format,
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        const std::vector<VkDeviceSize>& mipOffsets,
        VkImageUsageFlags usage,
        VkImageTiling tiling,
        VkMemoryPropertyFlags properties) const;

    [[nodiscard]]
    VkImageView createImageView(
        const std::shared_ptr<Image>& image,
        VkFormat format,
        VkImageAspectFlags imageAspect,
        uint32_t mipLevels) const;

    [[nodiscard]] VkFence createFence() const;
    [[nodiscard]] VkFence createFence(const VkFenceCreateInfo& createInfo) const;
    void destroyFence(VkFence& inOutFence) const;
    [[nodiscard]] VkResult waitForFence(VkFence fence, uint64_t timeout) const;
    [[nodiscard]] VkResult waitForFences(uint32_t count, const VkFence* fences, bool waitAll, uint64_t timeout) const;

    void bind(const std::shared_ptr<Buffer>& buffer) const;
    void map(const std::shared_ptr<Buffer>& buffer, void** data) const;
    void unmap(const std::shared_ptr<Buffer>& buffer) const;

    void waitIdle() const;

    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const;
    [[nodiscard]] VkDevice getLogicalDevice() const;
    [[nodiscard]] const std::shared_ptr<Queue>& getGraphicsQueue() const;
    [[nodiscard]] VkCommandPool getGraphicsCommandPool() const;
    [[nodiscard]] const std::shared_ptr<Queue>& getPresentationQueue() const;

    [[nodiscard]]
    uint32_t getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    SwapChainDesc buildSwapChainDesc(
        uint32_t width,
        uint32_t height,
        VkFormat desiredFormat,
        VkColorSpaceKHR desiredColorSpace);
    void querySwapChainSurfaceCapabilities(SwapChainDesc* swapChainDesc);
    void querySwapChainSurfaceFormats(SwapChainDesc* swapChainDesc);
    void querySwapChainPresentModes(SwapChainDesc* swapChainDesc);
    static void updateSwapChainExtent(
        uint32_t width, uint32_t
        height,
        SwapChainDesc* inOutSwapChainDesc);
    static void updateSwapChainSurfaceFormatAndColorSpace(
        VkFormat desiredFormat,
        VkColorSpaceKHR desiredColorSpace,
        SwapChainDesc* inOutSwapChainDesc);
    static void updateSwapChainPresentMode(SwapChainDesc* inOutSwapChainDesc);
    void createSwapChainInternal(const SwapChainDesc& swapChainDesc);

    void checkFormat(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);
    std::shared_ptr<Image> createDepthBufferImage(VkFormat format);

    void createCommandPool();
    void createBufferInternal(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags memoryProperties,
        VkBuffer& outBuffer,
        VkDeviceMemory& outDeviceMemory) const;

    [[nodiscard]]
    std::shared_ptr<Image> createImage(
        const ImageDesc& imageDesc,
        VkImageUsageFlags usage,
        VkImageTiling tiling,
        VkMemoryPropertyFlags properties) const;

    void transitionImageLayout(
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        uint32_t mipLevels) const;

    void updateImage(
        const std::shared_ptr<Image>& image,
        const std::vector<std::byte>& imageData,
        bool isGenerateMipmaps) const;

    void generateMipmaps(
        VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        uint32_t mipLevels) const;

    [[nodiscard]] VkSampler createTextureSampler(uint32_t mipLevels) const;

    GraphicsDeviceDesc desc {};
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    std::shared_ptr<Queue> graphicsQueue;
    std::shared_ptr<Queue> presentationQueue;
    VkSurfaceKHR presentSurface;
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<DepthBuffer> depthBuffer;
    VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;
};

} // namespace rfx