#include "rfx/pch.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/common/to.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace std;
namespace views = views;

// ---------------------------------------------------------------------------------------------------------------------

GraphicsDevice::GraphicsDevice(
    GraphicsDeviceDesc desc,
    VkPhysicalDevice physicalDevice,
    VkDevice logicalDevice,
    vector<uint32_t> usedQueueFamilyIndices,
    shared_ptr<Queue> graphicsQueue,
    shared_ptr<Queue> presentQueue,
    VkSurfaceKHR presentSurface,
    shared_ptr<Queue> computeQueue)
        : desc_(move(desc)),
          physicalDevice(physicalDevice),
          device(logicalDevice),
          usedQueueFamilyIndices(move(usedQueueFamilyIndices)),
          graphicsQueue(move(graphicsQueue)),
          presentationQueue(move(presentQueue)),
          presentSurface(presentSurface),
          computeQueue(move(computeQueue))
{
    createGraphicsCommandPool();
    createComputeCommandPool();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createGraphicsCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = 0, // Optional
        .queueFamilyIndex = graphicsQueue->getFamilyIndex()
    };

    ThrowIfFailed(vkCreateCommandPool(
        device,
        &poolInfo,
        nullptr,
        &graphicsCommandPool));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createComputeCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = computeQueue->getFamilyIndex()
    };

    ThrowIfFailed(vkCreateCommandPool(
        device,
        &poolInfo,
        nullptr,
        &computeCommandPool));
}

// ---------------------------------------------------------------------------------------------------------------------

GraphicsDevice::~GraphicsDevice()
{
    destroyMultiSamplingBuffer();
    destroyDepthBuffer();
    destroySwapChain();

    vkDestroyCommandPool(device, graphicsCommandPool, nullptr);
    vkDestroyCommandPool(device, computeCommandPool, nullptr);
    vkDestroyDevice(device, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyDepthBuffer()
{
    depthBuffer.reset();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroySwapChain()
{
    swapChain.reset();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyMultiSamplingBuffer()
{
    if (multiSampleImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device, multiSampleImageView, nullptr);
        multiSampleImageView = VK_NULL_HANDLE;
    }
    multiSampleImage.reset();
}

// ---------------------------------------------------------------------------------------------------------------------

VkPhysicalDevice GraphicsDevice::getPhysicalDevice() const
{
    return physicalDevice;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDevice GraphicsDevice::getLogicalDevice() const
{
    return device;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Queue>& GraphicsDevice::getGraphicsQueue() const
{
    return graphicsQueue;
}

// ---------------------------------------------------------------------------------------------------------------------

VkCommandPool GraphicsDevice::getGraphicsCommandPool() const
{
    return graphicsCommandPool;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Queue>& GraphicsDevice::getPresentationQueue() const
{
    return presentationQueue;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createSwapChain(
    uint32_t width,
    uint32_t height)
{
    createSwapChain(
        width,
        height,
        DEFAULT_SWAPCHAIN_FORMAT,
        DEFAULT_COLORSPACE);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createSwapChain(
    uint32_t width,
    uint32_t height,
    VkFormat desiredFormat,
    VkColorSpaceKHR desiredColorSpace)
{
    SwapChainDesc swapChainDesc = buildSwapChainDesc(width, height, desiredFormat, desiredColorSpace);

    createSwapChainInternal(swapChainDesc);
}

// ---------------------------------------------------------------------------------------------------------------------

SwapChainDesc GraphicsDevice::buildSwapChainDesc(
    uint32_t width,
    uint32_t height,
    VkFormat desiredFormat,
    VkColorSpaceKHR desiredColorSpace)
{
    SwapChainDesc swapChainDesc {};

    querySwapChainSurfaceCapabilities(&swapChainDesc);
    querySwapChainSurfaceFormats(&swapChainDesc);
    querySwapChainPresentModes(&swapChainDesc);

    updateSwapChainExtent(width, height, &swapChainDesc);
    updateSwapChainSurfaceFormatAndColorSpace(desiredFormat, desiredColorSpace, &swapChainDesc);
    updateSwapChainPresentMode(&swapChainDesc);

    return swapChainDesc;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainSurfaceCapabilities(SwapChainDesc* swapChainDesc)
{
    ThrowIfFailed(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        presentSurface,
        &swapChainDesc->surface.capabilities));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainSurfaceFormats(SwapChainDesc* swapChainDesc)
{
    uint32_t formatCount;
    ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        presentSurface,
        &formatCount,
        nullptr));

    if (formatCount != 0) {
        swapChainDesc->surface.formats.resize(formatCount);
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice,
            presentSurface,
            &formatCount,
            swapChainDesc->surface.formats.data()));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainPresentModes(SwapChainDesc* swapChainDesc)
{
    uint32_t presentationModesCount = 0;
    ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        presentSurface,
        &presentationModesCount,
        nullptr));

    if (presentationModesCount != 0) {
        swapChainDesc->surface.presentModes.resize(presentationModesCount);
        ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice,
            presentSurface,
            &presentationModesCount,
            swapChainDesc->surface.presentModes.data()));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::updateSwapChainExtent(
    uint32_t width,
    uint32_t height,
    SwapChainDesc* inOutSwapChainDesc)
{
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = inOutSwapChainDesc->surface.capabilities;

    if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
        inOutSwapChainDesc->extent = surfaceCapabilities.currentExtent;
        return;
    }

    VkExtent2D actualExtent = { width, height };
    actualExtent.width = max(
        surfaceCapabilities.minImageExtent.width,
        min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = max(
        surfaceCapabilities.minImageExtent.height,
        min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

    inOutSwapChainDesc->extent = actualExtent;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::updateSwapChainSurfaceFormatAndColorSpace(
    VkFormat desiredFormat,
    VkColorSpaceKHR desiredColorSpace,
    SwapChainDesc* inOutSwapChainDesc)
{
    const auto& surfaceFormats = inOutSwapChainDesc->surface.formats;
    RFX_CHECK_STATE(!surfaceFormats.empty(), "");
    RFX_CHECK_STATE(!(surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED), "");

    auto it = ranges::find_if(surfaceFormats,
        [desiredFormat, desiredColorSpace](const VkSurfaceFormatKHR& currentFormat) {
            return currentFormat.format == desiredFormat
                && currentFormat.colorSpace == desiredColorSpace;
        });

    if (it != surfaceFormats.end()) {
        inOutSwapChainDesc->format = desiredFormat;
        inOutSwapChainDesc->colorSpace = desiredColorSpace;
        return;
    }

    it = ranges::find_if(surfaceFormats,
        [desiredFormat](const VkSurfaceFormatKHR& currentFormat) {
            return currentFormat.format == desiredFormat;
        });

    if (it != surfaceFormats.end()) {
        RFX_LOG_WARNING << "Requested surface format isn't supported - different color space selected";
        inOutSwapChainDesc->format = it->format;
        inOutSwapChainDesc->colorSpace = it->colorSpace;
        return;
    }

    RFX_LOG_WARNING << "Requested surface format isn't supported - using first available format";
    inOutSwapChainDesc->format = surfaceFormats[0].format;
    inOutSwapChainDesc->colorSpace = surfaceFormats[0].colorSpace;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::updateSwapChainPresentMode(SwapChainDesc* inOutSwapChainDesc)
{
    auto it = ranges::find_if(inOutSwapChainDesc->surface.presentModes,
        [](const VkPresentModeKHR& presentMode) {
            return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
        });

    inOutSwapChainDesc->presentMode =
        it != inOutSwapChainDesc->surface.presentModes.end()
            ? *it
            : VK_PRESENT_MODE_FIFO_KHR;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createSwapChainInternal(const SwapChainDesc& swapChainDesc)
{
    destroyDepthBuffer();
    destroySwapChain();

    uint32_t imageCount = swapChainDesc.surface.capabilities.minImageCount + 1;
    if (swapChainDesc.surface.capabilities.maxImageCount > 0
            && imageCount > swapChainDesc.surface.capabilities.maxImageCount) {
        imageCount = swapChainDesc.surface.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = presentSurface,
        .minImageCount = imageCount,
        .imageFormat = swapChainDesc.format,
        .imageColorSpace = swapChainDesc.colorSpace,
        .imageExtent = swapChainDesc.extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = swapChainDesc.surface.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = swapChainDesc.presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    const uint32_t graphicsFamily = graphicsQueue->getFamilyIndex();
    const uint32_t presentFamily = presentationQueue->getFamilyIndex();
    uint32_t queueFamiliyIndices[] = {
        graphicsFamily,
        presentFamily
    };

    if (graphicsFamily != presentFamily) {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamiliyIndices;
    }
    else {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    VkSwapchainKHR vkSwapChain = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateSwapchainKHR(
        device,
        &swapChainCreateInfo,
        nullptr,
        &vkSwapChain));

    swapChain = make_unique<SwapChain>(device, vkSwapChain, swapChainDesc);
}

// ---------------------------------------------------------------------------------------------------------------------

const unique_ptr<SwapChain>& GraphicsDevice::getSwapChain() const
{
    return swapChain;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createDepthBuffer(VkFormat format)
{
    checkFormat(
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    shared_ptr<Image> image = createDepthBufferImage(format);
    VkImageView imageView = createImageView(image, format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    depthBuffer = make_unique<DepthBuffer>(device, image, imageView);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::checkFormat(
    VkFormat format,
    VkImageTiling tiling,
    VkFormatFeatureFlags features)
{
    VkFormatProperties formatProperties {};
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

    if (tiling == VK_IMAGE_TILING_LINEAR
            && (formatProperties.linearTilingFeatures & features) == features) {
        return;
    }

    if (tiling == VK_IMAGE_TILING_OPTIMAL
            && (formatProperties.optimalTilingFeatures & features) == features) {
        return;
    }

    RFX_THROW("Failed to find supported format");
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Image> GraphicsDevice::createDepthBufferImage(VkFormat format)
{
    RFX_CHECK_ARGUMENT(format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM);

    const SwapChainDesc& swapChainDesc = swapChain->getDesc();
    const uint32_t bytesPerPixel = format == VK_FORMAT_D16_UNORM ? 2 : 4;

    const ImageDesc imageDesc {
        .format = format,
        .width = swapChainDesc.extent.width,
        .height = swapChainDesc.extent.height,
        .bytesPerPixel = bytesPerPixel,
        .mipLevels = 1,
        .mipOffsets = { 0 },
        .sampleCount = multiSampleCount
    };

    return createImage(
        "depth_buffer",
        imageDesc,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

// ---------------------------------------------------------------------------------------------------------------------

const unique_ptr<DepthBuffer>& GraphicsDevice::getDepthBuffer() const
{
    return depthBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createMultiSamplingBuffer(VkSampleCountFlagBits sampleCount)
{
    destroyMultiSamplingBuffer();

    multiSampleCount = sampleCount;
    if (sampleCount == VK_SAMPLE_COUNT_1_BIT) {
        return;
    }

    const SwapChainDesc& swapChainDesc = swapChain->getDesc();
    RFX_CHECK_STATE(swapChainDesc.format == VK_FORMAT_B8G8R8A8_SRGB, "");

    const ImageDesc imageDesc {
        .format = swapChainDesc.format,
        .width = swapChainDesc.extent.width,
        .height = swapChainDesc.extent.height,
        .bytesPerPixel = 4,
        .mipLevels = 1,
        .mipOffsets = { 0 },
        .sampleCount = sampleCount
    };

    multiSampleImage = createImage(
        "multisampling_buffer",
        imageDesc,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    multiSampleImageView = createImageView(multiSampleImage, swapChainDesc.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

VkSampleCountFlagBits GraphicsDevice::getMultiSampleCount() const
{
    return multiSampleCount;
}

// ---------------------------------------------------------------------------------------------------------------------

VkImageView GraphicsDevice::getMultiSampleImageView() const
{
    return multiSampleImageView;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::waitIdle() const
{
    ThrowIfFailed(vkDeviceWaitIdle(device));
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Buffer> GraphicsDevice::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryProperties) const
{
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    createBufferInternal(
        size,
        usage,
        memoryProperties,
        false,
        vkBuffer,
        vkDeviceMemory);

    return make_shared<Buffer>(
        size,
        device,
        vkBuffer,
        vkDeviceMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createBufferInternal(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryProperties,
    bool shared,
    VkBuffer& outBuffer,
    VkDeviceMemory& outDeviceMemory) const
{
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    uint32_t queueFamilyIndexCount = 0;
    uint32_t* queueFamilyIndices = nullptr;

    if (shared) {
        queueFamilyIndexCount = static_cast<uint32_t>(usedQueueFamilyIndices.size());
        sharingMode = queueFamilyIndexCount > 1
            ? VK_SHARING_MODE_CONCURRENT
            : VK_SHARING_MODE_EXCLUSIVE;
        queueFamilyIndices = queueFamilyIndexCount > 1
            ? const_cast<uint32_t*>(usedQueueFamilyIndices.data())
            : nullptr;
    }

    VkBufferCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = sharingMode,
        .queueFamilyIndexCount = queueFamilyIndexCount,
        .pQueueFamilyIndices = queueFamilyIndices
    };

    ThrowIfFailed(vkCreateBuffer(
        device,
        &createInfo,
        nullptr,
        &outBuffer));

    VkMemoryRequirements memoryRequirements {};
    vkGetBufferMemoryRequirements(device, outBuffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = getMemoryType(memoryRequirements.memoryTypeBits, memoryProperties)
    };

    ThrowIfFailed(vkAllocateMemory(
        device,
        &allocInfo,
        nullptr,
        &outDeviceMemory));
}

// ---------------------------------------------------------------------------------------------------------------------

BufferPtr GraphicsDevice::createSharedBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryProperties) const
{
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    createBufferInternal(
        size,
        usage,
        memoryProperties,
        true,
        vkBuffer,
        vkDeviceMemory);

    return make_shared<Buffer>(
        size,
        device,
        vkBuffer,
        vkDeviceMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t GraphicsDevice::getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    RFX_THROW("Failed to find suitable memory type");
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<VertexBuffer> GraphicsDevice::createVertexBuffer(uint32_t vertexCount, const VertexFormat& vertexFormat) const
{
    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
    const VkDeviceSize bufferSize = vertexCount * vertexFormat.getVertexSize();

    createBufferInternal(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        false,
        vkBuffer,
        vkDeviceMemory);

    return make_shared<VertexBuffer>(
        vertexCount,
        vertexFormat,
        device,
        vkBuffer,
        vkDeviceMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<IndexBuffer> GraphicsDevice::createIndexBuffer(uint32_t indexCount, VkIndexType indexType)
{
    VkDeviceSize bufferSize = 0;

    switch (indexType) {
        case VK_INDEX_TYPE_UINT16:
            bufferSize = indexCount * sizeof(uint16_t);
            break;
        case VK_INDEX_TYPE_UINT32:
            bufferSize = indexCount * sizeof(uint32_t);
            break;
        case VK_INDEX_TYPE_UINT8_EXT:
            bufferSize = indexCount;
            break;
        default:
            RFX_CHECK_ARGUMENT(false);
    }

    VkBuffer vkBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    createBufferInternal(
        bufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        false,
        vkBuffer,
        vkDeviceMemory);

    return make_shared<IndexBuffer>(
        indexCount,
        indexType,
        bufferSize,
        device,
        vkBuffer,
        vkDeviceMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::bind(const shared_ptr<Buffer>& buffer) const
{
    ThrowIfFailed(vkBindBufferMemory(
        device,
        buffer->getHandle(),
        buffer->getDeviceMemory(),
        0));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::map(const shared_ptr<Buffer>& buffer, void** data) const
{
    ThrowIfFailed(vkMapMemory(device,
        buffer->getDeviceMemory(),
        0,
        buffer->getSize(),
        0,
        data));
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::unmap(const shared_ptr<Buffer>& buffer) const
{
    vkUnmapMemory(device, buffer->getDeviceMemory());
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<CommandBuffer> GraphicsDevice::createCommandBuffer(VkCommandPool commandPool) const
{
    return createCommandBuffers(commandPool, 1).at(0);
}

// ---------------------------------------------------------------------------------------------------------------------

vector<shared_ptr<CommandBuffer>> GraphicsDevice::createCommandBuffers(VkCommandPool commandPool, uint32_t count) const
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count
    };

    vector<VkCommandBuffer> vkCommandBuffers(count);
    ThrowIfFailed(vkAllocateCommandBuffers(
        device,
        &allocInfo,
        vkCommandBuffers.data()));

    return vkCommandBuffers
            | views::transform([this](const VkCommandBuffer& vkCommandBuffer)
                { return make_shared<CommandBuffer>(device, vkCommandBuffer); })
            | to<vector>();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::flush(
    const shared_ptr<CommandBuffer>& commandBuffer) const
{
    VkFence fence = createFence();
    graphicsQueue->submit(commandBuffer, fence);
    ThrowIfFailed(waitForFence(fence, DEFAULT_FENCE_TIMEOUT));
    destroyFence(fence);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyCommandBuffer(
    const shared_ptr<CommandBuffer>& commandBuffer,
    VkCommandPool commandPool) const
{
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer->getHandle());
}

// ---------------------------------------------------------------------------------------------------------------------

Texture2DPtr GraphicsDevice::createTexture2D(
    const string& id,
    const ImageDesc& imageDesc,
    const vector<std::byte>& imageData,
    bool isGenerateMipmaps) const
{
    const ImagePtr textureImage = createImage(id, imageDesc, imageData, isGenerateMipmaps);
    const ImageDesc finalImageDesc = textureImage->getDesc();

    VkImageView textureImageView = createImageView(
        textureImage,
        finalImageDesc.format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        finalImageDesc.mipLevels);

    SamplerDesc samplerDesc {
        .minFilter = VK_FILTER_LINEAR,
        .magFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .maxLod = static_cast<float>(finalImageDesc.mipLevels)
    };

    return createTexture2D(textureImage, textureImageView, samplerDesc);
}

// ---------------------------------------------------------------------------------------------------------------------

Texture2DPtr GraphicsDevice::createTexture2D(
    const shared_ptr<Image>& image,
    VkImageView const& imageView,
    const SamplerDesc& samplerDesc) const
{
    VkSampler textureSampler = createSampler(samplerDesc);

    return make_shared<Texture2D>(
        device,
        image,
        imageView,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        textureSampler);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Image> GraphicsDevice::createImage(
    const string& id,
    const ImageDesc& imageDesc,
    const vector<std::byte>& imageData,
    bool isGenerateMipmaps) const
{
    ImageDesc finalImageDesc = imageDesc;
    if (isGenerateMipmaps) {
        finalImageDesc.mipOffsets = { 0 };
        finalImageDesc.mipLevels =
            static_cast<uint32_t>(floor(log2(max(imageDesc.width, imageDesc.height)))) + 1;
    }

    ImagePtr image = createImage(
        id,
        finalImageDesc,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (isGenerateMipmaps) {
        transitionImageLayout(
            image->getHandle(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            finalImageDesc.mipLevels);
    }

    updateImage(image, imageData, isGenerateMipmaps);

    if (isGenerateMipmaps) {
        generateMipmaps(
            image->getHandle(),
            VK_FORMAT_R8G8B8A8_SRGB,
            finalImageDesc.width,
            finalImageDesc.height,
            finalImageDesc.mipLevels);
    }

    return image;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Image> GraphicsDevice::createImage(
    const string& id,
    const ImageDesc& imageDesc,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    VkMemoryPropertyFlags properties) const
{
    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkImageCreateFlags>(imageDesc.isCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0),
        .imageType = VK_IMAGE_TYPE_2D,
        .format = imageDesc.format,
        .extent = { imageDesc.width, imageDesc.height, 1 },
        .mipLevels = imageDesc.mipLevels,
        .arrayLayers = static_cast<uint32_t>(imageDesc.isCubemap ? 6 : 1),
        .samples = imageDesc.sampleCount,
        .tiling = tiling,
        .usage = usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = graphicsQueue->getFamilyIndex(),
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VkImage image = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateImage(
        device,
        &imageCreateInfo,
        nullptr,
        &image));

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(device, image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = getMemoryType(memoryRequirements.memoryTypeBits, properties)
    };

    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    ThrowIfFailed(vkAllocateMemory(
        device,
        &memoryAllocInfo,
        nullptr,
        &imageMemory));

    ThrowIfFailed(vkBindImageMemory(
        device,
        image,
        imageMemory,
        0));

    return make_shared<Image>(
        id,
        imageDesc,
        device,
        image,
        imageMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::updateImage(
    const shared_ptr<Image>& image,
    const vector<std::byte>& imageData,
    bool isGenerateMipmaps) const
{
    const size_t bufferSize = imageData.size() * sizeof(std::byte);
    const shared_ptr<Buffer> stagingBuffer =
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data = nullptr;
    bind(stagingBuffer);
    map(stagingBuffer, &data);
    memcpy(data, imageData.data(), bufferSize);
    unmap(stagingBuffer);

    const ImageDesc& imageDesc = image->getDesc();

    vector<VkBufferImageCopy> imageCopies;
    uint32_t offsetIndex = 0;

    for (uint32_t faceIndex = 0, faceCount = imageDesc.isCubemap ? 6 : 1;
         faceIndex < faceCount;
         ++faceIndex)
    {
        // TODO: fix ambiguity between miplevels vs. mipOffsets
        const uint32_t mipLevelCount = min(static_cast<size_t>(imageDesc.mipLevels), imageDesc.mipOffsets.size());

        for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel)
        {
            VkBufferImageCopy imageCopy {
                .bufferOffset = imageDesc.mipOffsets[offsetIndex],
                .imageSubresource {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mipLevel,
                    .baseArrayLayer = faceIndex,
                    .layerCount = 1
                },
                .imageExtent {
                    .width = imageDesc.width >> mipLevel,
                    .height = imageDesc.height >> mipLevel,
                    .depth = 1
                },
            };
            imageCopies.push_back(imageCopy);
            ++offsetIndex;
        }
    }

    const CommandBufferPtr commandBuffer = createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin();

    if (!isGenerateMipmaps) {
        commandBuffer->setImageMemoryBarrier(
            image,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_HOST_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT);
//        commandBuffer->setImageMemoryBarrier(
//            image,
//            VK_IMAGE_LAYOUT_UNDEFINED,
//            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }

    commandBuffer->copyBufferToImage(stagingBuffer, image, imageCopies);

    if (!isGenerateMipmaps) {
        commandBuffer->setImageMemoryBarrier(
            image,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
//        commandBuffer->setImageMemoryBarrier(
//            image,
//            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    commandBuffer->end();

    VkFence fence = createFence();
    graphicsQueue->submit(commandBuffer, fence);
    ThrowIfFailed(waitForFence(fence, DEFAULT_FENCE_TIMEOUT));
    destroyFence(fence);
    destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::generateMipmaps(
    VkImage image,
    VkFormat imageFormat,
    int32_t texWidth,
    int32_t texHeight,
    uint32_t mipLevels) const
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);
    RFX_CHECK_STATE(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT,
        "Texture image format does not support linear blitting");

    const shared_ptr<CommandBuffer>& commandBuffer = createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageMemoryBarrier barrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer->getHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        VkImageBlit blit {
            .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i - 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .srcOffsets = {
                { 0, 0, 0 },
                { mipWidth, mipHeight, 1 }
            },
            .dstSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .dstOffsets = {
                { 0, 0, 0 },
                { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 }
            }
        };

        vkCmdBlitImage(commandBuffer->getHandle(),
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer->getHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer->getHandle(),
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    commandBuffer->end();
    graphicsQueue->submit(commandBuffer);
    graphicsQueue->waitIdle();
}

// ---------------------------------------------------------------------------------------------------------------------

VkImageView GraphicsDevice::createImageView(
    const ImagePtr& image,
    VkFormat format,
    VkImageAspectFlags imageAspect,
    uint32_t mipLevels) const
{
    const ImageDesc& imageDesc = image->getDesc();
    
    VkImageViewCreateInfo viewInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image->getHandle(),
        .viewType = imageDesc.isCubemap ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A
        },
        .subresourceRange = {
            .aspectMask = imageAspect,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = static_cast<uint32_t>(imageDesc.isCubemap ? 6 : 1)
        }
    };

    VkImageView imageView = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateImageView(
        device,
        &viewInfo,
        nullptr,
        &imageView));

    return imageView;
}

// ---------------------------------------------------------------------------------------------------------------------

VkSampler GraphicsDevice::createSampler(const SamplerDesc& desc) const
{
    VkSamplerCreateInfo samplerCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = desc.magFilter,
        .minFilter = desc.minFilter,
        .mipmapMode = desc.mipmapMode,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0f,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0.0f,
        .maxLod = desc.maxLod
    };

    if (desc_.features.samplerAnisotropy) {
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = desc_.properties.limits.maxSamplerAnisotropy;
    }

    VkSampler textureSampler = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateSampler(
        device,
        &samplerCreateInfo,
        nullptr,
        &textureSampler));

    return textureSampler;
}

// ---------------------------------------------------------------------------------------------------------------------

VkFence GraphicsDevice::createFence() const
{
    VkFenceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    return createFence(createInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

VkFence GraphicsDevice::createFence(const VkFenceCreateInfo& createInfo) const
{
    VkFence fence = VK_NULL_HANDLE;

    ThrowIfFailed(vkCreateFence(
        device,
        &createInfo,
        nullptr,
        &fence));

    return fence;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyFence(VkFence& inOutFence) const
{
    vkDestroyFence(device, inOutFence, nullptr);
    inOutFence = VK_NULL_HANDLE;
}

// ---------------------------------------------------------------------------------------------------------------------

VkResult GraphicsDevice::waitForFence(VkFence fence, uint64_t timeout) const
{
    return waitForFences(1, &fence, false, timeout);
}

// ---------------------------------------------------------------------------------------------------------------------

VkResult GraphicsDevice::waitForFences(uint32_t count, const VkFence* fences, bool waitAll, uint64_t timeout) const
{
    return vkWaitForFences(device, count, fences, waitAll ? VK_TRUE : VK_FALSE, timeout);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::transitionImageLayout(
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    uint32_t mipLevels) const
{
    const shared_ptr<CommandBuffer>& commandBuffer = createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageMemoryBarrier barrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
            && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw invalid_argument("unsupported layout transition");
    }

    vkCmdPipelineBarrier(
        commandBuffer->getHandle(),
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    commandBuffer->end();
    graphicsQueue->submit(commandBuffer);
    graphicsQueue->waitIdle();
}

// ---------------------------------------------------------------------------------------------------------------------

const GraphicsDeviceDesc& GraphicsDevice::getDesc() const
{
    return desc_;
}

// ---------------------------------------------------------------------------------------------------------------------

CubeMapPtr GraphicsDevice::createCubeMap(
    const string& id,
    const ImageDesc& imageDesc,
    const vector<std::byte>& imageData,
    bool isGenerateMipmaps) const
{
    const ImagePtr image = createImage(id, imageDesc, imageData, isGenerateMipmaps);
    const ImageDesc finalImageDesc = image->getDesc();

    VkImageView imageView = createImageView(
        image,
        finalImageDesc.format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        finalImageDesc.mipLevels);

    SamplerDesc samplerDesc {
        .minFilter = VK_FILTER_LINEAR,
        .magFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .maxLod = static_cast<float>(finalImageDesc.mipLevels)
    };

    VkSampler textureSampler = createSampler(samplerDesc);

    return make_shared<CubeMap>(
        device,
        image,
        imageView,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        textureSampler);
}

// ---------------------------------------------------------------------------------------------------------------------

VkCommandPool GraphicsDevice::getComputeCommandPool() const
{
    return computeCommandPool;
}

// ---------------------------------------------------------------------------------------------------------------------

const QueuePtr& GraphicsDevice::getComputeQueue() const
{
    return computeQueue;
}

// ---------------------------------------------------------------------------------------------------------------------
