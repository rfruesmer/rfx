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
    shared_ptr<Queue> graphicsQueue,
    shared_ptr<Queue> presentQueue,
    VkSurfaceKHR presentSurface)
        : desc(move(desc)),
          physicalDevice(physicalDevice),
          device(logicalDevice),
          graphicsQueue(move(graphicsQueue)),
          presentationQueue(move(presentQueue)),
          presentSurface(presentSurface)
{
    createCommandPool();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createCommandPool()
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

GraphicsDevice::~GraphicsDevice()
{
    destroyMultiSamplingBuffer();
    destroyDepthBuffer();
    destroySwapChain();

    vkDestroyCommandPool(device, graphicsCommandPool, nullptr);
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
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();

    return createImage(
        format,
        swapChainDesc.extent.width,
        swapChainDesc.extent.height,
        1,
        { 0 },
        multiSampleCount,
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
    multiSampleImage = createImage(
        swapChainDesc.format,
        swapChainDesc.extent.width,
        swapChainDesc.extent.height,
        1,
        {0},
        sampleCount,
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
    VkBuffer& outBuffer,
    VkDeviceMemory& outDeviceMemory) const
{
    VkBufferCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
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
            bufferSize = indexCount * 2;
            break;
        case VK_INDEX_TYPE_UINT32:
            bufferSize = indexCount * 4;
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
    const std::shared_ptr<CommandBuffer>& commandBuffer) const
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

shared_ptr<Texture2D> GraphicsDevice::createTexture2D(
    const ImageDesc& imageDesc,
    const vector<byte>& imageData,
    bool isGenerateMipmaps) const
{
    shared_ptr<Image> textureImage = createImage(
        imageDesc,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (isGenerateMipmaps) {
        transitionImageLayout(
            textureImage->getHandle(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            imageDesc.mipLevels);
    }

    updateImage(textureImage, imageData, isGenerateMipmaps);

    if (isGenerateMipmaps) {
        generateMipmaps(
            textureImage->getHandle(),
            VK_FORMAT_R8G8B8A8_SRGB,
            imageDesc.width,
            imageDesc.height,
            imageDesc.mipLevels);
    }

    VkImageView textureImageView = createImageView(
        textureImage,
        imageDesc.format,
        VK_IMAGE_ASPECT_COLOR_BIT,
        imageDesc.mipLevels);

    VkSampler textureSampler = createTextureSampler(imageDesc.mipLevels);

    return make_shared<Texture2D>(
        device,
        textureImage,
        textureImageView,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        textureSampler);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Image> GraphicsDevice::createImage(
    const ImageDesc& imageDesc,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    VkMemoryPropertyFlags properties) const
{
    return createImage(
        imageDesc.format,
        imageDesc.width,
        imageDesc.height,
        imageDesc.mipLevels,
        imageDesc.mipOffsets,
        imageDesc.sampleCount,
        usage,
        tiling,
        properties);
}

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<Image> GraphicsDevice::createImage(
    VkFormat format,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    const std::vector<VkDeviceSize>& mipOffsets,
    VkSampleCountFlagBits sampleCount,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    VkMemoryPropertyFlags properties) const
{
    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = { width, height, 1 },
        .mipLevels = mipLevels,
        .arrayLayers = 1,
        .samples = sampleCount,
        .tiling = tiling,
        .usage = usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = graphicsQueue->getFamilyIndex(),
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VkImage image = nullptr;
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

    VkDeviceMemory imageMemory = nullptr;
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

    ImageDesc imageDesc {
        .format = format,
        .width = width,
        .height = height,
        .mipLevels = mipLevels,
        .mipOffsets = mipOffsets
    };

    return make_shared<Image>(
        imageDesc,
        device,
        image,
        imageMemory);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::updateImage(
    const shared_ptr<Image>& image,
    const vector<byte>& imageData,
    bool isGenerateMipmaps) const
{
    const size_t bufferSize = imageData.size() * sizeof(byte);
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

    vector<VkBufferImageCopy> copyRegions;
    for (uint32_t i = 0; i < imageDesc.mipOffsets.size(); ++i) {
        VkBufferImageCopy bufferImageCopy {
            .bufferOffset = imageDesc.mipOffsets[i],
            .imageSubresource {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .imageExtent {
                .width = imageDesc.width >> i,
                .height = imageDesc.height >> i,
                .depth = 1
            },
        };
        copyRegions.push_back(bufferImageCopy);
    }

    const shared_ptr<CommandBuffer> commandBuffer = createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    if (!isGenerateMipmaps) {
        commandBuffer->setImageMemoryBarrier(
            image,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_HOST_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT);
    }

    commandBuffer->copyBufferToImage(stagingBuffer, image, copyRegions);

    if (!isGenerateMipmaps) {
        commandBuffer->setImageMemoryBarrier(
            image,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
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
    const shared_ptr<Image>& image,
    VkFormat format,
    VkImageAspectFlags imageAspect,
    uint32_t mipLevels) const
{
    VkImageViewCreateInfo viewInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image->getHandle(),
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = imageAspect,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
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

VkSampler GraphicsDevice::createTextureSampler(uint32_t mipLevels) const
{
    VkSamplerCreateInfo samplerInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0f,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0.0f,
        .maxLod = static_cast<float>(mipLevels),
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    if (desc.features.samplerAnisotropy) {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = desc.properties.limits.maxSamplerAnisotropy;
    }

    VkSampler textureSampler = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateSampler(
        device,
        &samplerInfo,
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
    VkFence fence = nullptr;

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
    inOutFence = nullptr;
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
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------

