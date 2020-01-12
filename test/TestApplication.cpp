#include "rfx/pch.h"
#include "test/TestApplication.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

TestApplication::TestApplication(filesystem::path configurationPath, handle_t instanceHandle)
#ifdef _WINDOWS
    : Win32Application(configurationPath, instanceHandle)
#else
static_assert(false, "not implemented yet");
#endif // _WINDOWS
{
    glslang::InitializeProcess();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initialize()
{
#ifdef _WINDOWS
    Win32Application::initialize();
#else
    static_assert(false, "not implemented yet");
#endif

    initCommandPool();
    initCommandBuffers();
    initRenderPass();
    initFrameBuffers();

    initScene();
    initCamera();
    initPipelineLayout();
    initDescriptorSet();
    initPipeline();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initCamera()
{
    modelMatrix = mat4(1.0f);
    cameraPosition = vec3(0.0f, 0.0f, 10.0f);
    cameraLookAt = vec3(0.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    projectionMatrix = perspective(radians(45.0f), 1.0f, 0.1f, 100.0f);

    uniformBuffer = graphicsDevice->createUniformBuffer(sizeof(modelViewProjMatrix));

    updateModelViewProjection();

    uniformBuffer->bind();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateModelViewProjection()
{
    viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUp);
    modelViewProjMatrix = projectionMatrix * viewMatrix * modelMatrix;
    uniformBuffer->load(sizeof(modelViewProjMatrix), reinterpret_cast<std::byte*>(&modelViewProjMatrix));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &layoutBinding;

    descriptorSetLayout = graphicsDevice->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initPipelineLayout()
{
    RFX_CHECK_STATE(descriptorSetLayout != nullptr, "descriptorSetLayout must be created before");

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    pipelineLayout = graphicsDevice->createPipelineLayout(pipelineLayoutCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initDescriptorPool(const vector<VkDescriptorPoolSize>& poolSizes)
{
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

    descriptorPool = graphicsDevice->createDescriptorPool(descriptorPoolCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initCommandPool()
{
    commandPool = graphicsDevice->createCommandPool(
        graphicsDevice->getDeviceInfo().graphicsQueueFamilyIndex);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initRenderPass()
{
    VkAttachmentDescription attachments[2];
    attachments[0].format = graphicsDevice->getSwapChainFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = 0;

    attachments[1].format = GraphicsDevice::DEPTHBUFFER_FORMAT;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].flags = 0;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthReference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.attachmentCount = 2;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    renderPass = graphicsDevice->createRenderPass(renderPassCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initFrameBuffers()
{
    const GraphicsDeviceInfo& deviceInfo = graphicsDevice->getDeviceInfo();

    VkImageView attachments[2];
    attachments[1] = graphicsDevice->getDepthBuffer().imageView;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = nullptr;
    frameBufferCreateInfo.renderPass = renderPass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = attachments;
    frameBufferCreateInfo.width = graphicsDevice->getSwapChainProperties().imageSize.width;
    frameBufferCreateInfo.height = graphicsDevice->getSwapChainProperties().imageSize.height;
    frameBufferCreateInfo.layers = 1;

    const vector<SwapChainBuffer>& swapChainBuffers = graphicsDevice->getSwapChainBuffers();
    frameBuffers.resize(swapChainBuffers.size());

    for (size_t i = 0, count = swapChainBuffers.size(); i < count; ++i) {
        attachments[0] = swapChainBuffers[i].imageView;
        frameBuffers[i] = graphicsDevice->createFrameBuffer(frameBufferCreateInfo);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineDynamicStateCreateInfo TestApplication::createDynamicState(uint32_t dynamicStateCount, VkDynamicState dynamicStates[])
{
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.dynamicStateCount = dynamicStateCount;
    dynamicState.pDynamicStates = dynamicStates;

    return dynamicState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineInputAssemblyStateCreateInfo TestApplication::createInputAssemblyState()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    return inputAssemblyState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineRasterizationStateCreateInfo TestApplication::createRasterizationState()
{
    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.pNext = nullptr;
    rasterizationState.flags = 0;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthBiasConstantFactor = 0;
    rasterizationState.depthBiasClamp = 0;
    rasterizationState.depthBiasSlopeFactor = 0;
    rasterizationState.lineWidth = 1.0f;

    return rasterizationState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineColorBlendAttachmentState TestApplication::createColorBlendAttachmentState()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = 0xf;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    return colorBlendAttachmentState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineColorBlendStateCreateInfo TestApplication::createColorBlendState(
    const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState)
{
    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.flags = 0;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendState.blendConstants[0] = 1.0f;
    colorBlendState.blendConstants[1] = 1.0f;
    colorBlendState.blendConstants[2] = 1.0f;
    colorBlendState.blendConstants[3] = 1.0f;

    return colorBlendState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineViewportStateCreateInfo TestApplication::createViewportState()
{
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = NUM_VIEWPORTS;
    viewportState.scissorCount = NUM_SCISSORS;
    viewportState.pScissors = nullptr;
    viewportState.pViewports = nullptr;

    return viewportState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineDepthStencilStateCreateInfo TestApplication::createDepthStencilState()
{
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.pNext = nullptr;
    depthStencilState.flags = 0;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0;
    depthStencilState.maxDepthBounds = 0;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.back.compareMask = 0;
    depthStencilState.back.reference = 0;
    depthStencilState.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.writeMask = 0;
    depthStencilState.front = depthStencilState.back;

    return depthStencilState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineMultisampleStateCreateInfo TestApplication::createMultiSampleState()
{
    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = {};
    multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleStateCreateInfo.pNext = nullptr;
    multiSampleStateCreateInfo.flags = 0;
    multiSampleStateCreateInfo.pSampleMask = nullptr;
    multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multiSampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multiSampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    multiSampleStateCreateInfo.minSampleShading = 0.0;

    return multiSampleStateCreateInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::update()
{
    static const float MOVE_DELTA = 0.005f;

    Win32Application::update();

    bool cameraNeedsUpdate = false;

    if (keyboard->isKeyDown(Keyboard::KEY_W)) {
        cameraPosition.z -= MOVE_DELTA;
        cameraLookAt.z -= MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_S)) {
        cameraPosition.z += MOVE_DELTA;
        cameraLookAt.z += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_A)) {
        cameraPosition.x -= MOVE_DELTA;
        cameraLookAt.x -= MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_D)) {
        cameraPosition.x += MOVE_DELTA;
        cameraLookAt.x += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_UP)) {
        cameraPosition.y -= MOVE_DELTA;
        cameraLookAt.y -= MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_DOWN)) {
        cameraPosition.y += MOVE_DELTA;
        cameraLookAt.y += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_ESCAPE)) {
        PostQuitMessage(0);
    }

    if (cameraNeedsUpdate) {
        updateModelViewProjection();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::draw()
{
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    VkSemaphore imageAcquiredSemaphore = graphicsDevice->createSemaphore(imageAcquiredSemaphoreCreateInfo);
    uint32_t nextImageIndex = UINT32_MAX;
    VkResult result = graphicsDevice->acquireNextSwapChainImage(
        UINT64_MAX, imageAcquiredSemaphore, nullptr, nextImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) {
        windowResized = false;
        recreateSwapChain();
        graphicsDevice->destroySemaphore(imageAcquiredSemaphore);
        return;
    }
    else if (result != VK_SUCCESS) {
        RFX_CHECK_STATE(false, "Failed to acquire next swap chain image");
    }

    VkFenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;
    VkFence drawFence = graphicsDevice->createFence(fenceCreateInfo);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkCommandBuffer vkCommandBuffers[] = { renderCommandBuffers[nextImageIndex]->getHandle() };

    VkSubmitInfo submitInfo = {};
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = vkCommandBuffers;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    graphicsDevice->getGraphicsQueue()->submit(1, &submitInfo, drawFence);

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &graphicsDevice->getSwapChain();
    presentInfo.pImageIndices = &nextImageIndex;
    presentInfo.pWaitSemaphores = nullptr;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pResults = nullptr;

    do {
        result = graphicsDevice->waitForFences(1, &drawFence, true, DEFAULT_FENCE_TIMEOUT);
    } while (result == VK_TIMEOUT);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to execute command buffer");

    graphicsDevice->getPresentQueue()->present(presentInfo);

    graphicsDevice->destroySemaphore(imageAcquiredSemaphore);
    graphicsDevice->destroyFence(drawFence);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::shutdown()
{
    destroyFrameBuffers();
    freeCommandBuffers();
    destroyPipeline();
    destroyPipelineLayout();
    destroyRenderPass();
    destroyDescriptors();
    destroyBuffers();

    glslang::FinalizeProcess();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyFrameBuffers()
{
    for (auto& frameBuffer : frameBuffers) {
        graphicsDevice->destroyFrameBuffer(frameBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::freeCommandBuffers() const
{
    commandPool->freeAllCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyPipeline()
{
    graphicsDevice->destroyPipeline(pipeline);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyPipelineLayout()
{
    graphicsDevice->destroyPipelineLayout(pipelineLayout);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyRenderPass()
{
    graphicsDevice->destroyRenderPass(renderPass);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyDescriptors()
{
    graphicsDevice->destroyDescriptorPool(descriptorPool);
    graphicsDevice->destroyDescriptorSetLayout(descriptorSetLayout);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroyBuffers() const
{
    uniformBuffer->dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::recreateSwapChain()
{
    graphicsDevice->waitIdle();

    destroyFrameBuffers();
    freeCommandBuffers();
    destroyPipeline();
    destroyRenderPass();
    destroySwapChainAndDepthBuffer();

    createSwapChainAndDepthBuffer();
    initRenderPass();
    initPipeline();
    initFrameBuffers();
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::destroySwapChainAndDepthBuffer() const
{
    graphicsDevice->destroyDepthBuffer();
    graphicsDevice->destroySwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createSwapChainAndDepthBuffer() const
{
    graphicsDevice->createSwapChain();
    graphicsDevice->createDepthBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------
