#include "rfx/pch.h"
#include "ColoredQuadTest.h"
#include "rfx/application/ShaderLoader.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace rfx::test;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<ColoredQuadTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::initGraphics()
{
    Application::initGraphics();

    // TODO: extract to virtual initScene (or similar)
    createRenderPass();
    createDescriptorSetLayout();
    buildScene();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createRenderPass()
{
    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();
    const VkSampleCountFlagBits multiSampleCount = graphicsDevice->getMultiSampleCount();

    VkAttachmentDescription colorAttachment {
        .format = swapChainDesc.format,
        .samples = multiSampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = devToolsEnabled || multiSampleCount > VK_SAMPLE_COUNT_1_BIT
            ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentRef {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription colorAttachmentResolve {
        .format = swapChainDesc.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = devToolsEnabled
            ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentResolveRef {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const unique_ptr<DepthBuffer>& depthBuffer = graphicsDevice->getDepthBuffer();
    VkAttachmentDescription depthAttachment {};
    if (depthBuffer) {
        depthAttachment = {
            .format = depthBuffer->getFormat(),
            .samples = multiSampleCount,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };
    }

    VkAttachmentReference depthAttachmentRef {
        .attachment = static_cast<uint32_t>(multiSampleCount > VK_SAMPLE_COUNT_1_BIT ? 2 : 1),
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pResolveAttachments = multiSampleCount > VK_SAMPLE_COUNT_1_BIT ? &colorAttachmentResolveRef : nullptr,
        .pDepthStencilAttachment = depthBuffer ? &depthAttachmentRef : nullptr
    };

    VkSubpassDependency subpassDependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                      | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                      | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                       | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    };

    vector<VkAttachmentDescription> attachments { colorAttachment };
    if (multiSampleCount > VK_SAMPLE_COUNT_1_BIT) {
        attachments.push_back(colorAttachmentResolve);
    }
    if (depthBuffer) {
        attachments.push_back(depthAttachment);
    }

    VkRenderPassCreateInfo renderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency
    };

    ThrowIfFailed(vkCreateRenderPass(
        graphicsDevice->getLogicalDevice(),
        &renderPassCreateInfo,
        nullptr,
        &renderPass));
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::buildScene()
{
    const path assetsDirectory = getAssetsDirectory();
    const path vertexShaderPath = assetsDirectory / "shaders/color.vert";
    const path fragmentShaderPath = assetsDirectory / "shaders/color.frag";

    const ShaderLoader shaderLoader(graphicsDevice);
    vertexShader = shaderLoader.loadVertexShader(
        vertexShaderPath,
        "main",
        VertexFormat(VertexFormat::COORDINATES | VertexFormat::COLORS_4));
    fragmentShader = shaderLoader.loadFragmentShader(fragmentShaderPath, "main");

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createVertexBuffer()
{
    struct Vertex {
        glm::vec3 pos;
        glm::vec4 color;
    };

    const vector<Vertex> vertices = {
        {{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
        {{  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }}
    };

    const VertexFormat vertexFormat(VertexFormat::COORDINATES | VertexFormat::COLORS_4);
    const VkDeviceSize bufferSize = vertices.size() * vertexFormat.getVertexSize();
    shared_ptr<Buffer> stagingBuffer = graphicsDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice->bind(stagingBuffer);
    graphicsDevice->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, vertices.data(), stagingBuffer->getSize());
    graphicsDevice->unmap(stagingBuffer);

    vertexBuffer = graphicsDevice->createVertexBuffer(vertices.size(), vertexFormat);
    graphicsDevice->bind(vertexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, vertexBuffer);
    commandBuffer->end();

    const shared_ptr<Queue>& graphicsQueue = graphicsDevice->getGraphicsQueue();
    graphicsQueue->submit(commandBuffer);
    graphicsQueue->waitIdle();

    graphicsDevice->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createIndexBuffer()
{
    const vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    const VkDeviceSize bufferSize = indices.size() * sizeof(uint16_t);
    shared_ptr<Buffer> stagingBuffer = graphicsDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mappedMemory = nullptr;
    graphicsDevice->bind(stagingBuffer);
    graphicsDevice->map(stagingBuffer, &mappedMemory);
    memcpy(mappedMemory, indices.data(), stagingBuffer->getSize());
    graphicsDevice->unmap(stagingBuffer);

    indexBuffer = graphicsDevice->createIndexBuffer(indices.size(), VK_INDEX_TYPE_UINT16);
    graphicsDevice->bind(indexBuffer);

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = graphicsDevice->createCommandBuffer(graphicsCommandPool);
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    commandBuffer->copyBuffer(stagingBuffer, indexBuffer);
    commandBuffer->end();

    const shared_ptr<Queue>& graphicsQueue = graphicsDevice->getGraphicsQueue();
    graphicsQueue->submit(commandBuffer);
    graphicsQueue->waitIdle();

    graphicsDevice->destroyCommandBuffer(commandBuffer, graphicsCommandPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    const size_t bufferCount = graphicsDevice->getSwapChain()->getDesc().bufferCount;

    uniformBuffers.resize(bufferCount);

    for (size_t i = 0; i < bufferCount; ++i) {
        uniformBuffers[i] =
            graphicsDevice->createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice->bind(uniformBuffers[i]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createDescriptorPool()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    VkDescriptorPoolSize poolSize {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = swapChainDesc.bufferCount
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = swapChainDesc.bufferCount,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool));
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createDescriptorSets()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    vector<VkDescriptorSetLayout> layouts(swapChainDesc.bufferCount, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = swapChainDesc.bufferCount,
        .pSetLayouts = layouts.data()
    };

    descriptorSets.resize(swapChainDesc.bufferCount);

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        &allocInfo,
        descriptorSets.data()));

    VkDescriptorBufferInfo bufferInfo {
        .offset = 0,
        .range = sizeof(UniformBufferObject)
    };

    VkWriteDescriptorSet descriptorWrite {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr, // Optional
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = nullptr // Optional
    };

    for (uint32_t i = 0; i < swapChainDesc.bufferCount; ++i) {
        bufferInfo.buffer = uniformBuffers[i]->getHandle();
        descriptorWrite.dstSet = descriptorSets[i];

        vkUpdateDescriptorSets(graphicsDevice->getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr // Optional
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &uniformBufferLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &layoutInfo,
        nullptr,
        &descriptorSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createGraphicsPipeline()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapChainDesc.extent.width),
        .height = static_cast<float>(swapChainDesc.extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = swapChainDesc.extent
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = graphicsDevice->getMultiSampleCount(),
        .sampleShadingEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                          | VK_COLOR_COMPONENT_G_BIT
                          | VK_COLOR_COMPONENT_B_BIT
                          | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {}, // Optional
        .back = {}, // Optional
        .minDepthBounds = 0.0f, // Optional
        .maxDepthBounds = 1.0f, // Optional
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0
    };

    ThrowIfFailed(vkCreatePipelineLayout(
        graphicsDevice->getLogicalDevice(),
        &pipelineLayoutInfo,
        nullptr,
        &pipelineLayout));

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertexShader->getStageCreateInfo(),
        fragmentShader->getStageCreateInfo()
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexShader->getVertexInputStateCreateInfo(),
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = &depthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = nullptr, // Optional
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1 // Optional
    };

    ThrowIfFailed(vkCreateGraphicsPipelines(
        graphicsDevice->getLogicalDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &defaultPipeline));
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::createCommandBuffers()
{
    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();
    const vector<VkFramebuffer>& swapChainFramebuffers = swapChain->getFramebuffers();
    const unique_ptr<DepthBuffer>& depthBuffer = graphicsDevice->getDepthBuffer();

    vector<VkClearValue> clearValues(1);
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (graphicsDevice->getMultiSampleCount() > VK_SAMPLE_COUNT_1_BIT) {
        clearValues.resize(clearValues.size() + 1);
        clearValues[clearValues.size() - 1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (depthBuffer) {
        clearValues.resize(clearValues.size() + 1);
        clearValues[clearValues.size() - 1].depthStencil = { 1.0f, 0 };
    }

    const vector<shared_ptr<VertexBuffer>> vertexBuffers = { vertexBuffer };

    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .renderArea = {
            .offset = { 0, 0 },
            .extent = swapChainDesc.extent
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()
    };

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    commandBuffers = graphicsDevice->createCommandBuffers(graphicsCommandPool, swapChainFramebuffers.size());

    for (size_t i = 0; i < commandBuffers.size(); ++i) {

        renderPassBeginInfo.framebuffer = swapChainFramebuffers[i];

        const auto& commandBuffer = commandBuffers[i];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPassBeginInfo);
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, defaultPipeline);
        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSets[i]);
        commandBuffer->bindVertexBuffers(vertexBuffers);
        commandBuffer->bindIndexBuffer(indexBuffer);
        commandBuffer->drawIndexed(indexBuffer->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::update(float deltaTime)
{
    static auto startTime = chrono::high_resolution_clock::now();

    auto currentTime = chrono::high_resolution_clock::now();
    float time = chrono::duration<float, chrono::seconds::period>(currentTime - startTime).count();

    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    UniformBufferObject ubo {
        .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        .view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        .proj = glm::perspective(glm::radians(45.0f), swapChainDesc.extent.width / (float) swapChainDesc.extent.height, 0.1f, 10.0f)
    };
    ubo.proj[1][1] *= -1;

    void* mappedMemory = nullptr;
    graphicsDevice->map(uniformBuffers[currentImageIndex], &mappedMemory);
    memcpy(mappedMemory, &ubo, sizeof(ubo));
    graphicsDevice->unmap(uniformBuffers[currentImageIndex]);
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::cleanup()
{
    vkDestroyDescriptorSetLayout(graphicsDevice->getLogicalDevice(), descriptorSetLayout, nullptr);
    vertexBuffer.reset();
    indexBuffer.reset();
    vertexShader.reset();
    fragmentShader.reset();

    Application::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::cleanupSwapChain()
{
    uniformBuffers.clear();

    Application::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void ColoredQuadTest::recreateSwapChain()
{
    Application::recreateSwapChain();

    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();

    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

