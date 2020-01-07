#include "rfx/pch.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "GraphicsContext.h"


using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

bool memoryTypeFromProperties(GraphicsDeviceInfo& deviceInfo, 
    uint32_t typeBits,
    VkFlags requirementsMask,
    uint32_t* typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < deviceInfo.memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((deviceInfo.memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

GraphicsDevice::GraphicsDevice(VkDevice vkLogicalDevice,
    VkPhysicalDevice vkPhysicalDevice,
    VkSurfaceKHR presentationSurface,
    GraphicsDeviceInfo deviceInfo,
    const shared_ptr<Window>& window,
    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr,
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR)
        : vkDevice(vkLogicalDevice),
          vkPhysicalDevice(vkPhysicalDevice),
          presentationSurface(presentationSurface),
          deviceInfo(move(deviceInfo)),
          window(window),
          vkGetDeviceProcAddr(vkGetDeviceProcAddr),
          vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR),
          vkGetPhysicalDeviceSurfaceFormatsKHR(vkGetPhysicalDeviceSurfaceFormatsKHR) {}

// ---------------------------------------------------------------------------------------------------------------------

GraphicsDevice::~GraphicsDevice()
{
    dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::dispose()
{
    destroyCommandPools();
    destroyDepthBuffer();
    destroySwapChain();
    destroyDevice();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyCommandPools()
{
    unordered_set<shared_ptr<CommandPool>> copyOfCommandPools(commandPools);

    for (const auto& it : copyOfCommandPools) {
        destroyCommandPool(it);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyDevice()
{
    if (vkDevice)
    {
        vkDestroyDevice(vkDevice, nullptr);
        vkDevice = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroySwapChain()
{
    for (auto& swapChainBuffer : swapChainBuffers) {
        vkDestroyImageView(vkDevice, swapChainBuffer.imageView, nullptr);
    }
    swapChainBuffers.clear();

    if (swapChain) {
        vkDestroySwapchainKHR(vkDevice, swapChain, nullptr);
        swapChain = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyDepthBuffer()
{
    if (depthBuffer.imageView) {
        vkDestroyImageView(vkDevice, depthBuffer.imageView, nullptr);
        depthBuffer.imageView = nullptr;
    }

    if (depthBuffer.image) {
        vkDestroyImage(vkDevice, depthBuffer.image, nullptr);
        depthBuffer.image = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::initialize()
{
    loadDeviceFunctions();
    createDefaultQueues();
    createSwapChain();
    createDepthBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::loadDeviceFunctions()
{
#define LOAD_DEVICE_FUNCTION(name)                                                   \
    name = (PFN_##name)vkGetDeviceProcAddr(vkDevice, #name);                            \
    vk.##name = name;                                                                       \
    RFX_CHECK_STATE(name != nullptr, string("Failed to load function: ") + (#name));

    memset(&vk, 0, sizeof(vk));

    LOAD_DEVICE_FUNCTION(vkGetDeviceQueue);
    LOAD_DEVICE_FUNCTION(vkDeviceWaitIdle);
    LOAD_DEVICE_FUNCTION(vkDestroyDevice);

    LOAD_DEVICE_FUNCTION(vkCreateSemaphore);
    LOAD_DEVICE_FUNCTION(vkDestroySemaphore);

    LOAD_DEVICE_FUNCTION(vkCreateFence);
    LOAD_DEVICE_FUNCTION(vkWaitForFences);
    LOAD_DEVICE_FUNCTION(vkDestroyFence);

    LOAD_DEVICE_FUNCTION(vkQueueSubmit);
    LOAD_DEVICE_FUNCTION(vkQueuePresentKHR);

    LOAD_DEVICE_FUNCTION(vkCreateRenderPass);
    LOAD_DEVICE_FUNCTION(vkDestroyRenderPass);

    LOAD_DEVICE_FUNCTION(vkCreateCommandPool);
    LOAD_DEVICE_FUNCTION(vkDestroyCommandPool);
    LOAD_DEVICE_FUNCTION(vkAllocateCommandBuffers);
    LOAD_DEVICE_FUNCTION(vkBeginCommandBuffer);
    LOAD_DEVICE_FUNCTION(vkEndCommandBuffer);
    LOAD_DEVICE_FUNCTION(vkResetCommandBuffer);
    LOAD_DEVICE_FUNCTION(vkFreeCommandBuffers);
    LOAD_DEVICE_FUNCTION(vkCmdBeginRenderPass);
    LOAD_DEVICE_FUNCTION(vkCmdEndRenderPass);
    LOAD_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
    LOAD_DEVICE_FUNCTION(vkCmdBindPipeline);
    LOAD_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
    LOAD_DEVICE_FUNCTION(vkCmdSetViewport);
    LOAD_DEVICE_FUNCTION(vkCmdSetScissor);
    LOAD_DEVICE_FUNCTION(vkCmdDraw);

    LOAD_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
    LOAD_DEVICE_FUNCTION(vkBindImageMemory);
    LOAD_DEVICE_FUNCTION(vkCreateImage);
    LOAD_DEVICE_FUNCTION(vkDestroyImage);
    LOAD_DEVICE_FUNCTION(vkCreateImageView);
    LOAD_DEVICE_FUNCTION(vkDestroyImageView);

    LOAD_DEVICE_FUNCTION(vkAllocateMemory);
    LOAD_DEVICE_FUNCTION(vkFreeMemory);
    LOAD_DEVICE_FUNCTION(vkCreateBuffer);
    LOAD_DEVICE_FUNCTION(vkDestroyBuffer);
    LOAD_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
    LOAD_DEVICE_FUNCTION(vkMapMemory);
    LOAD_DEVICE_FUNCTION(vkUnmapMemory);
    LOAD_DEVICE_FUNCTION(vkBindBufferMemory);

    LOAD_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
    LOAD_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
    LOAD_DEVICE_FUNCTION(vkCreateDescriptorPool);
    LOAD_DEVICE_FUNCTION(vkAllocateDescriptorSets);
    LOAD_DEVICE_FUNCTION(vkUpdateDescriptorSets);
    LOAD_DEVICE_FUNCTION(vkDestroyDescriptorPool);
    LOAD_DEVICE_FUNCTION(vkCreatePipelineLayout);
    LOAD_DEVICE_FUNCTION(vkDestroyPipelineLayout);

    LOAD_DEVICE_FUNCTION(vkCreateShaderModule);
    LOAD_DEVICE_FUNCTION(vkDestroyShaderModule);

    LOAD_DEVICE_FUNCTION(vkCreateFramebuffer);
    LOAD_DEVICE_FUNCTION(vkDestroyFramebuffer);

    LOAD_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
    LOAD_DEVICE_FUNCTION(vkDestroyPipeline);

    LOAD_DEVICE_FUNCTION(vkCreateSwapchainKHR);
    LOAD_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
    LOAD_DEVICE_FUNCTION(vkAcquireNextImageKHR);
    LOAD_DEVICE_FUNCTION(vkDestroySwapchainKHR);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createDefaultQueues()
{
    graphicsQueue = createQueue(deviceInfo.graphicsQueueFamilyIndex);

    if (deviceInfo.graphicsQueueFamilyIndex != deviceInfo.presentQueueFamilyIndex) {
        presentQueue = createQueue(deviceInfo.presentQueueFamilyIndex);
    }
    else {
        presentQueue = graphicsQueue;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Queue> GraphicsDevice::createQueue(uint32_t queueFamilyIndex) const
{
    VkQueue vulkanQueue = nullptr;

    vkGetDeviceQueue(vkDevice, queueFamilyIndex, 0, &vulkanQueue);
    RFX_CHECK_STATE(vulkanQueue != nullptr,
        "Failed to get device queue");

    return make_shared<Queue>(vulkanQueue, vk);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainProperties()
{
    querySwapChainSurfaceCapabilities();
    querySwapChainSurfaceFormats();
    querySwapChainImageSize();
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainSurfaceCapabilities()
{
    const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vkPhysicalDevice, presentationSurface, &swapChainProperties.surfaceCapabilities);
    RFX_CHECK_STATE(result == VK_SUCCESS,
        "Failed to get presentation capabilities");

    const VkSurfaceCapabilitiesKHR& surfaceCaps = swapChainProperties.surfaceCapabilities;
    swapChainProperties.imageCount = surfaceCaps.minImageCount + 1;
    if (surfaceCaps.maxImageCount > 0 && swapChainProperties.imageCount > surfaceCaps.maxImageCount) {
        swapChainProperties.imageCount = surfaceCaps.maxImageCount;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainSurfaceFormats()
{
    uint32_t formatsCount = 0;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, presentationSurface, &formatsCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS,
        "Failed to get number of supported presentation surface formats");

    if (formatsCount == 0) {
        return;
    }

    swapChainProperties.surfaceFormats.resize(formatsCount);

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, presentationSurface, &formatsCount,
        &swapChainProperties.surfaceFormats[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && formatsCount > 0,
        "Failed to get supported presentation surface formats");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::querySwapChainImageSize()
{
    const VkSurfaceCapabilitiesKHR& surfaceCaps = swapChainProperties.surfaceCapabilities;

    if (surfaceCaps.currentExtent.width != 0xFFFFFFFF) {
        swapChainProperties.imageSize = surfaceCaps.currentExtent;
    }
    else {
        swapChainProperties.imageSize = {
            static_cast<uint32_t>(window->getWidth()),
            static_cast<uint32_t>(window->getHeight())
        };

        if (swapChainProperties.imageSize.width < surfaceCaps.minImageExtent.width) {
            swapChainProperties.imageSize.width = surfaceCaps.minImageExtent.width;
        }
        else if (swapChainProperties.imageSize.width > surfaceCaps.maxImageExtent.width) {
            swapChainProperties.imageSize.width = surfaceCaps.maxImageExtent.width;
        }

        if (swapChainProperties.imageSize.height < surfaceCaps.minImageExtent.height) {
            swapChainProperties.imageSize.height = surfaceCaps.minImageExtent.height;
        }
        else if (swapChainProperties.imageSize.height > surfaceCaps.maxImageExtent.height) {
            swapChainProperties.imageSize.height = surfaceCaps.maxImageExtent.height;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createDepthBuffer()
{
    VkImageCreateInfo imageCreateInfo = {};

    if (deviceInfo.depthBufferFormatProperties.linearTilingFeatures 
            & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    }
    else if (deviceInfo.depthBufferFormatProperties.optimalTilingFeatures 
            & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    }
    else {
        RFX_THROW("VK_FORMAT_D16_UNORM Unsupported.");
    }

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = DEPTHBUFFER_FORMAT;
    imageCreateInfo.extent.width = swapChainProperties.imageSize.width;
    imageCreateInfo.extent.height = swapChainProperties.imageSize.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.flags = 0;

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = 0;
    allocateInfo.memoryTypeIndex = 0;

    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.pNext = nullptr;
    viewCreateInfo.image = nullptr;
    viewCreateInfo.format = DEPTHBUFFER_FORMAT;
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.flags = 0;

    depthBuffer.format = DEPTHBUFFER_FORMAT;

    VkResult result = vkCreateImage(vkDevice, &imageCreateInfo, nullptr, &depthBuffer.image);
    RFX_CHECK_STATE(result == VK_SUCCESS && depthBuffer.image != nullptr, 
        "Failed to create depth buffer image");

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(vkDevice, depthBuffer.image, &memoryRequirements);

    allocateInfo.allocationSize = memoryRequirements.size;

    const bool succeeded = memoryTypeFromProperties(deviceInfo, memoryRequirements.memoryTypeBits, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocateInfo.memoryTypeIndex);
    RFX_CHECK_STATE(succeeded, "Failed to determine memory type ");

    result = vkAllocateMemory(vkDevice, &allocateInfo, nullptr, &depthBuffer.deviceMemory);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to allocate depth buffer memory");

    result = vkBindImageMemory(vkDevice, depthBuffer.image, depthBuffer.deviceMemory, 0);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to bind depth buffer memory");

    viewCreateInfo.image = depthBuffer.image;
    result = vkCreateImageView(vkDevice, &viewCreateInfo, nullptr, &depthBuffer.imageView);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to create image view for depth buffer");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::waitIdle() const
{
    const VkResult result = vkDeviceWaitIdle(vkDevice);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to wait for device idle state");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createSwapChain()
{
    createSwapChain(DEFAULT_SWAPCHAIN_FORMAT, DEFAULT_COLORSPACE);
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::createSwapChain(VkFormat desiredFormat, VkColorSpaceKHR desiredColorSpace)
{
    swapChainFormat = desiredFormat;
    VkColorSpaceKHR colorSpace = desiredColorSpace;

    querySwapChainProperties();
    getSwapChainSurfaceFormat(swapChainFormat, colorSpace);
    const VkSurfaceTransformFlagBitsKHR preTransform = getSwapChainTransform();
    const VkCompositeAlphaFlagBitsKHR compositeAlpha = getSwapChainCompositeAlpha();

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.flags = 0;
    swapChainCreateInfo.surface = deviceInfo.presentSurface;
    swapChainCreateInfo.minImageCount = swapChainProperties.imageCount;
    swapChainCreateInfo.imageFormat = swapChainFormat;
    swapChainCreateInfo.imageColorSpace = colorSpace;
    swapChainCreateInfo.imageExtent = swapChainProperties.imageSize;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.preTransform = preTransform;
    swapChainCreateInfo.compositeAlpha = compositeAlpha;
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = swapChain;

    uint32_t queueFamilyIndices[2] = { deviceInfo.graphicsQueueFamilyIndex, deviceInfo.presentQueueFamilyIndex };
    if (deviceInfo.graphicsQueueFamilyIndex != deviceInfo.presentQueueFamilyIndex) {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    VkResult result = vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &swapChain);
    RFX_CHECK_STATE(result == VK_SUCCESS && swapChain != nullptr,
        "Failed to crate swapchain");

    if (swapChainCreateInfo.oldSwapchain != nullptr) {
        vkDestroySwapchainKHR(vkDevice, swapChainCreateInfo.oldSwapchain, nullptr);
    }

    uint32_t imageCount = 0;
    result = vkGetSwapchainImagesKHR(vkDevice, swapChain, &imageCount, nullptr);
    RFX_CHECK_STATE(result == VK_SUCCESS && imageCount > 0,
        "Failed to get number of swapchain images");

    const auto swapChainImages = static_cast<VkImage*>(malloc(imageCount * sizeof(VkImage)));

    result = vkGetSwapchainImagesKHR(vkDevice, swapChain, &imageCount, &swapChainImages[0]);
    RFX_CHECK_STATE(result == VK_SUCCESS && imageCount > 0,
        "Failed to get number of swapchain images");

    swapChainBuffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        swapChainBuffers[i].image = swapChainImages[i];
    }

    free(swapChainImages);

    for (uint32_t i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.pNext = nullptr;
        viewCreateInfo.flags = 0;
        viewCreateInfo.image = swapChainBuffers[i].image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = swapChainFormat;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(vkDevice, &viewCreateInfo, nullptr, &swapChainBuffers[i].imageView);
        RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to create image view for swapchain");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::getSwapChainSurfaceFormat(VkFormat& inOutFormat, VkColorSpaceKHR& inOutColorSpace) const
{
    const auto& presentationSurfaceFormats = swapChainProperties.surfaceFormats;

    if (presentationSurfaceFormats.size() == 1
        && presentationSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        return;
    }

    for (const auto& currentFormat : presentationSurfaceFormats) {
        if (inOutFormat == currentFormat.format
            && inOutColorSpace == currentFormat.colorSpace) {
            return;
        }
    }

    for (const auto& currentFormat : presentationSurfaceFormats) {
        if (inOutFormat == currentFormat.format) {
            inOutColorSpace = currentFormat.colorSpace;
            RFX_LOG_WARNING << "Requested surface format isn't supported - different color space selected";
            return;
        }
    }

    RFX_LOG_WARNING << "Requested surface format isn't supported - using first available format";

    inOutFormat = presentationSurfaceFormats[0].format;
    inOutColorSpace = presentationSurfaceFormats[0].colorSpace;
}

// ---------------------------------------------------------------------------------------------------------------------

VkSurfaceTransformFlagBitsKHR GraphicsDevice::getSwapChainTransform() const
{
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (swapChainProperties.surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
        preTransform = swapChainProperties.surfaceCapabilities.currentTransform;
    }

    return preTransform;
}

// ---------------------------------------------------------------------------------------------------------------------

VkCompositeAlphaFlagBitsKHR GraphicsDevice::getSwapChainCompositeAlpha() const
{
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    for (auto& compositeAlphaFlag : compositeAlphaFlags)
    {
        if (swapChainProperties.surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) {
            compositeAlpha = compositeAlphaFlag;
            break;
        }
    }

    return compositeAlpha;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<CommandPool> GraphicsDevice::createCommandPool(uint32_t queueFamilyIndex)
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool vkCommandPool = nullptr;
    const VkResult result = vkCreateCommandPool(vkDevice, &createInfo, nullptr, &vkCommandPool);
    RFX_CHECK_STATE(result == VK_SUCCESS && vkCommandPool != nullptr,
        "Failed to create command pool");

    shared_ptr<CommandPool> commandPool = make_shared<CommandPool>(vkDevice, vkCommandPool, vk);
    commandPools.insert(commandPool);

    return commandPool;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyCommandPool(const shared_ptr<CommandPool>& commandPool)
{
    if (commandPool->isValid()) {
        waitIdle();

        const VkCommandPool vkCommandPool = commandPool->getHandle();
        commandPool->clear();
        commandPool->invalidate();
        commandPools.erase(commandPool);

        vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Buffer> GraphicsDevice::createUniformBuffer(size_t size)
{
    return createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Buffer> GraphicsDevice::createVertexBuffer(size_t size)
{
    return createBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Buffer> GraphicsDevice::createBuffer(size_t size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.size = size;
    bufferCreateInfo.queueFamilyIndexCount = deviceInfo.graphicsQueueFamilyIndex;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    VkBuffer vkBuffer = nullptr;

    VkResult result = vkCreateBuffer(vkDevice, &bufferCreateInfo, nullptr, &vkBuffer);
    RFX_CHECK_STATE(result == VK_SUCCESS && vkBuffer != nullptr,
        "Failed to create uniform buffer");

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(vkDevice, vkBuffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.allocationSize = memoryRequirements.size;

    const bool success = memoryTypeFromProperties(deviceInfo, memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &allocateInfo.memoryTypeIndex);
    RFX_CHECK_STATE(success, "No mappable, coherent memory");

    VkDeviceMemory vkDeviceMemory = nullptr;
    result = vkAllocateMemory(vkDevice, &allocateInfo, nullptr, &vkDeviceMemory);
    RFX_CHECK_STATE(result == VK_SUCCESS && vkDeviceMemory != nullptr,
        "Failed to allocated memory for uniform buffer");

    return make_shared<Buffer>(vkDevice, vkBuffer, vkDeviceMemory, vk, memoryRequirements.size, size);
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSetLayout GraphicsDevice::createDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo) const
{
    VkDescriptorSetLayout descriptorSetLayout = nullptr;

    const VkResult result = vkCreateDescriptorSetLayout(vkDevice, &createInfo, nullptr, &descriptorSetLayout);
    RFX_CHECK_STATE(result == VK_SUCCESS && descriptorSetLayout != nullptr, 
        "Failed to create descriptor set layout");

    return descriptorSetLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyDescriptorSetLayout(VkDescriptorSetLayout& inOutDescriptorSetLayout) const
{
    vkDestroyDescriptorSetLayout(vkDevice, inOutDescriptorSetLayout, nullptr);
    inOutDescriptorSetLayout = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::allocateDescriptorSets(const VkDescriptorSetAllocateInfo& allocateInfo,
    vector<VkDescriptorSet>& outDescriptorSets) const
{
    outDescriptorSets.resize(allocateInfo.descriptorSetCount);

    const VkResult result = vkAllocateDescriptorSets(vkDevice, &allocateInfo, outDescriptorSets.data());
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to allocated descriptor sets");
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::updateDescriptorSets(size_t count, const VkWriteDescriptorSet* writes) const
{
    vkUpdateDescriptorSets(vkDevice, static_cast<uint32_t>(count), writes, 0, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorPool GraphicsDevice::createDescriptorPool(const VkDescriptorPoolCreateInfo& createInfo) const
{
    VkDescriptorPool descriptorPool = nullptr;

    const VkResult result = vkCreateDescriptorPool(vkDevice, &createInfo, nullptr, &descriptorPool);
    RFX_CHECK_STATE(result == VK_SUCCESS && descriptorPool != nullptr,
        "Failed to create descriptor pool");

    return descriptorPool;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyDescriptorPool(VkDescriptorPool& inOutDescriptorPool) const
{
    vkDestroyDescriptorPool(vkDevice, inOutDescriptorPool, nullptr);
    inOutDescriptorPool = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineLayout GraphicsDevice::createPipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) const
{
    VkPipelineLayout pipelineLayout = nullptr;

    const VkResult result = vkCreatePipelineLayout(vkDevice, &createInfo, nullptr, &pipelineLayout);
    RFX_CHECK_STATE(result == VK_SUCCESS && pipelineLayout != nullptr,
        "Failed to create pipeline layout");

    return pipelineLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyPipelineLayout(VkPipelineLayout& inOutPipelineLayout) const
{
    vkDestroyPipelineLayout(vkDevice, inOutPipelineLayout, nullptr);
    inOutPipelineLayout = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkSemaphore GraphicsDevice::createSemaphore(const VkSemaphoreCreateInfo& createInfo) const
{
    VkSemaphore semaphore = nullptr;

    const VkResult result = vkCreateSemaphore(vkDevice, &createInfo, nullptr, &semaphore);
    RFX_CHECK_STATE(result == VK_SUCCESS && semaphore != nullptr,
        "Failed to create semaphore");

    return semaphore;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroySemaphore(VkSemaphore& inOutSemaphore) const
{
    vkDestroySemaphore(vkDevice, inOutSemaphore, nullptr);
    inOutSemaphore = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkResult GraphicsDevice::acquireNextSwapChainImage(uint64_t timeout, 
    VkSemaphore semaphore, 
    VkFence fence, 
    uint32_t& outImageIndex) const
{
    outImageIndex = UINT32_MAX;

    const VkResult result = vkAcquireNextImageKHR(vkDevice, swapChain, timeout, semaphore, fence, &outImageIndex);
    if (result != VK_SUCCESS) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RFX_LOG_WARNING << "Failed to acquire next swap chain image due to: VK_ERROR_OUT_OF_DATE_KHR";
        }
        else if (result == VK_SUBOPTIMAL_KHR) {
            RFX_LOG_WARNING << "Failed to acquire next swap chain image due to: VK_SUBOPTIMAL_KHR";
        }
        else {
            RFX_LOG_WARNING << "Failed to acquire next swap chain image due to: " << result;
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------------------

const SwapChainProperties& GraphicsDevice::getSwapChainProperties() const
{
    return swapChainProperties;
}

// ---------------------------------------------------------------------------------------------------------------------

VkFormat GraphicsDevice::getSwapChainFormat() const
{
    return swapChainFormat;
}

// ---------------------------------------------------------------------------------------------------------------------

VkRenderPass GraphicsDevice::createRenderPass(const VkRenderPassCreateInfo& createInfo) const
{
    VkRenderPass renderPass = nullptr;

    const VkResult result = vkCreateRenderPass(vkDevice, &createInfo, nullptr, &renderPass);
    RFX_CHECK_STATE(result == VK_SUCCESS && renderPass != nullptr,
        "Failed to create render pass");

    return renderPass;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyRenderPass(VkRenderPass& inOutRenderPass) const
{
    vkDestroyRenderPass(vkDevice, inOutRenderPass, nullptr);
    inOutRenderPass = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkShaderModule GraphicsDevice::createShaderModule(const VkShaderModuleCreateInfo& createInfo) const
{
    VkShaderModule shaderModule = nullptr;

    const VkResult result = vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule);
    RFX_CHECK_STATE(result == VK_SUCCESS && shaderModule != nullptr, "");

    return shaderModule;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyShaderModule(VkShaderModule& inOutShaderModule) const
{
    vkDestroyShaderModule(vkDevice, inOutShaderModule, nullptr);
    inOutShaderModule = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkFramebuffer GraphicsDevice::createFrameBuffer(const VkFramebufferCreateInfo& createInfo) const
{
    VkFramebuffer frameBuffer = nullptr;

    const VkResult result = vkCreateFramebuffer(vkDevice, &createInfo, nullptr, &frameBuffer);
    RFX_CHECK_STATE(result == VK_SUCCESS && frameBuffer != nullptr, 
        "Failed to create framebuffer");

    return frameBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyFrameBuffer(VkFramebuffer& inOutFrameBuffer) const
{
    vkDestroyFramebuffer(vkDevice, inOutFrameBuffer, nullptr);
    inOutFrameBuffer = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkFence GraphicsDevice::createFence(const VkFenceCreateInfo& createInfo) const
{
    VkFence fence = nullptr;

    const VkResult result = vkCreateFence(vkDevice, &createInfo, nullptr, &fence);
    RFX_CHECK_STATE(result == VK_SUCCESS && fence != nullptr,
        "Failed to create fence");

    return fence;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyFence(VkFence& inOutFence) const
{
    vkDestroyFence(vkDevice, inOutFence, nullptr);
    inOutFence = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

VkResult GraphicsDevice::waitForFences(uint32_t count, const VkFence* fences, bool waitAll, uint64_t timeout) const
{
    return vkWaitForFences(vkDevice, count, fences, waitAll ? VK_TRUE : VK_FALSE, timeout);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline GraphicsDevice::createGraphicsPipeline(const VkGraphicsPipelineCreateInfo& createInfo) const
{
    VkPipeline pipeline = nullptr;

    const VkResult result = vkCreateGraphicsPipelines(vkDevice, nullptr, 1, &createInfo, nullptr, &pipeline);
    RFX_CHECK_STATE(result == VK_SUCCESS && pipeline != nullptr,
        "Failed to create graphics pipeline");

    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

void GraphicsDevice::destroyPipeline(VkPipeline& inOutPipeline) const
{
    vkDestroyPipeline(vkDevice, inOutPipeline, nullptr);
    inOutPipeline = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

const GraphicsDeviceInfo& GraphicsDevice::getDeviceInfo() const
{
    return deviceInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

const VkSwapchainKHR& GraphicsDevice::getSwapChain() const
{
    return swapChain;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<SwapChainBuffer>& GraphicsDevice::getSwapChainBuffers() const
{
    return swapChainBuffers;
}

// ---------------------------------------------------------------------------------------------------------------------

const DepthBuffer& GraphicsDevice::getDepthBuffer() const
{
    return depthBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Queue>& GraphicsDevice::getGraphicsQueue() const
{
    return graphicsQueue;
}

// ---------------------------------------------------------------------------------------------------------------------

const shared_ptr<Queue>& GraphicsDevice::getPresentQueue() const
{
    return presentQueue;
}

// ---------------------------------------------------------------------------------------------------------------------