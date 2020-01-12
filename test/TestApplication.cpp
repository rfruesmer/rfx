#include "rfx/pch.h"
#include "test/TestApplication.h"
#include "rfx/graphics/SPIR.h"
#include "rfx/core/FileUtil.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

TestApplication::TestApplication(HINSTANCE instanceHandle)
    : Win32Application(instanceHandle)
{
    glslang::InitializeProcess();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initialize()
{
    Win32Application::initialize();

    initCamera();
    initPipelineLayout();
    initDescriptorPool();
    initDescriptorSet();
    initRenderPass();
    initVertexShaderModule();
    initFragmentShaderModule();
    initFrameBuffers();
    initVertexBuffer();
    initPipeline();
    initCommandPool();
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initCamera()
{
    modelMatrix = mat4(1.0f);
    cameraPosition = vec3(0.0f, 0.0f, 10.0f);
    cameraLookAt = vec3(0.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    projectionMatrix = perspective(radians(45.0f), 1.0f, 0.1f, 100.0f);
    clipMatrix = mat4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f);

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

void TestApplication::initCommandPool()
{
    commandPool = graphicsDevice->createCommandPool(
        graphicsDevice->getDeviceInfo().graphicsQueueFamilyIndex);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initCommandBuffers()
{
    commandBuffers = commandPool->allocateCommandBuffers(graphicsDevice->getSwapChainBuffers().size());

    const VkExtent2D presentImageSize = graphicsDevice->getSwapChainProperties().imageSize;

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.2f;
    clearValues[0].color.float32[1] = 0.2f;
    clearValues[0].color.float32[2] = 0.2f;
    clearValues[0].color.float32[3] = 0.2f;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(presentImageSize.width);
    viewport.height = static_cast<float>(presentImageSize.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.extent.width = presentImageSize.width;
    scissor.extent.height = presentImageSize.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    for (size_t i = 0, count = commandBuffers.size(); i < count; ++i) {
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = frameBuffers[i];
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent = presentImageSize;
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        auto& commandBuffer = commandBuffers[i];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, descriptorSets);
        commandBuffer->bindVertexBuffers({ vertexBuffer });
        commandBuffer->setViewport(viewport);
        commandBuffer->setScissor(scissor);
        commandBuffer->draw(getVertexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
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

void TestApplication::initVertexShaderModule()
{
    string vertexShaderString;
    FileUtil::readTextFile(getVertexShaderPath(), vertexShaderString);
    vector<unsigned int> vertexShaderSPV;
    GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderString.c_str(), vertexShaderSPV);

    shaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfos[0].pNext = nullptr;
    shaderStageCreateInfos[0].pSpecializationInfo = nullptr;
    shaderStageCreateInfos[0].flags = 0;
    shaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfos[0].pName = "main";

    VkShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;
    shaderModuleCreateInfo.codeSize = vertexShaderSPV.size() * sizeof(unsigned int);
    shaderModuleCreateInfo.pCode = vertexShaderSPV.data();

    shaderStageCreateInfos[0].module = graphicsDevice->createShaderModule(shaderModuleCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initFragmentShaderModule()
{
    string fragmentShaderString;
    FileUtil::readTextFile(getFragmentShaderPath(), fragmentShaderString);
    vector<unsigned int> fragmentShaderSPV;
    GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderString.c_str(), fragmentShaderSPV);

    shaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfos[1].pNext = nullptr;
    shaderStageCreateInfos[1].pSpecializationInfo = nullptr;
    shaderStageCreateInfos[1].flags = 0;
    shaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfos[1].pName = "main";

    VkShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;
    shaderModuleCreateInfo.codeSize = fragmentShaderSPV.size() * sizeof(unsigned int);
    shaderModuleCreateInfo.pCode = fragmentShaderSPV.data();

    shaderStageCreateInfos[1].module = graphicsDevice->createShaderModule(shaderModuleCreateInfo);
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

void TestApplication::initVertexBuffer()
{
    const std::byte* vertexData = getVertexData();
    const size_t vertexSize = getVertexSize();
    const size_t vertexCount = getVertexCount();
    const size_t vertexDataSize = vertexCount * vertexSize;

    vertexBuffer = graphicsDevice->createVertexBuffer(vertexDataSize);
    vertexBuffer->load(vertexDataSize, vertexData);
    vertexBuffer->bind();

    vertexInputBinding.binding = 0;
    vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBinding.stride = getVertexSize();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initPipeline()
{
    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE] = {};
    dynamicStateEnables[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStateEnables[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext = nullptr;
    dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables;
    dynamicStateCreateInfo.dynamicStateCount = 2;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBinding;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributes;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext = nullptr;
    inputAssemblyStateCreateInfo.flags = 0;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0;
    rasterizationStateCreateInfo.depthBiasClamp = 0;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = 0xf;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.pNext = nullptr;
    colorBlendStateCreateInfo.flags = 0;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendStateCreateInfo.blendConstants[0] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 1.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 1.0f;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = NUM_VIEWPORTS;
    viewportStateCreateInfo.scissorCount = NUM_SCISSORS;
    viewportStateCreateInfo.pScissors = nullptr;
    viewportStateCreateInfo.pViewports = nullptr;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext = nullptr;
    depthStencilStateCreateInfo.flags = 0;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = 0;
    depthStencilStateCreateInfo.maxDepthBounds = 0;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilStateCreateInfo.back.compareMask = 0;
    depthStencilStateCreateInfo.back.reference = 0;
    depthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.back.writeMask = 0;
    depthStencilStateCreateInfo.front = depthStencilStateCreateInfo.back;

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

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.pStages = shaderStageCreateInfos;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipeline = graphicsDevice->createGraphicsPipeline(pipelineCreateInfo);
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
        cameraPosition.y += MOVE_DELTA;
        cameraLookAt.y += MOVE_DELTA;
        cameraNeedsUpdate = true;
    }

    if (keyboard->isKeyDown(Keyboard::KEY_DOWN)) {
        cameraPosition.y -= MOVE_DELTA;
        cameraLookAt.y -= MOVE_DELTA;
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
    VkCommandBuffer vkCommandBuffers[] = { commandBuffers[nextImageIndex]->getHandle() };

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
        result = graphicsDevice->waitForFences(1, &drawFence, true, FENCE_TIMEOUT);
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
    destroyShaderModules();
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

void TestApplication::destroyShaderModules()
{
    graphicsDevice->destroyShaderModule(shaderStageCreateInfos[0].module);
    graphicsDevice->destroyShaderModule(shaderStageCreateInfos[1].module);
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
    vertexBuffer->destroy();
    uniformBuffer->destroy();
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
