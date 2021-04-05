#include "rfx/pch.h"
#include "TexturedQuadTest.h"
#include "rfx/graphics/TextureLoader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace rfx::test;
using namespace glm;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<TexturedQuadTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::initGraphics()
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

void TexturedQuadTest::createRenderPass()
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

void TexturedQuadTest::buildScene()
{
    const path assetsDirectory = getAssetsDirectory();
    const path vertexShaderPath = assetsDirectory / "shaders/texture.vert";
    const path fragmentShaderPath = assetsDirectory / "shaders/texture.frag";

    const ShaderLoader shaderLoader(graphicsDevice);
    vertexShader = shaderLoader.loadVertexShader(
        vertexShaderPath,
        "main",
        VertexFormat(VertexFormat::COORDINATES | VertexFormat::TEXCOORDS),
        vector<string>(),
        vector<string>(),
        vector<string>());
    fragmentShader = shaderLoader.loadFragmentShader(
        fragmentShaderPath,
        "main",
        vector<string>(),
        vector<string>());

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createTexture();
    createDescriptorPool();
    createDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::createVertexBuffer()
{
    struct Vertex {
        vec3 pos;
        vec2 texCoord;
    };

    const vector<Vertex> vertices = {
        {{ -1.0f,  1.0f,  0.0f}, { 0.0f, 0.0f }},
        {{ -1.0f, -1.0f,  0.0f}, { 0.0f, 1.0f }},
        {{  1.0f, -1.0f,  0.0f}, { 1.0f, 1.0f }},
        {{  1.0f,  1.0f,  0.0f}, { 1.0f, 0.0f }}
    };

    const VertexFormat vertexFormat(VertexFormat::COORDINATES | VertexFormat::TEXCOORDS);
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

void TexturedQuadTest::createIndexBuffer()
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

void TexturedQuadTest::createUniformBuffers()
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

void TexturedQuadTest::createDescriptorPool()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    array<VkDescriptorPoolSize, 2> poolSizes {};
    poolSizes[0] = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = static_cast<uint32_t>(swapChainDesc.bufferCount)
    };

    poolSizes[1] = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = static_cast<uint32_t>(swapChainDesc.bufferCount)
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = swapChainDesc.bufferCount,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::createDescriptorSets()
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

    VkDescriptorImageInfo imageInfo {
        .sampler = texture->getSampler(),
        .imageView = texture->getImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    array<VkWriteDescriptorSet, 2> descriptorWrites {};
    descriptorWrites[0] = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };

    descriptorWrites[1] = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo
    };

    for (uint32_t i = 0; i < swapChainDesc.bufferCount; ++i) {
        bufferInfo.buffer = uniformBuffers[i]->getHandle();
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[1].dstSet = descriptorSets[i];

        vkUpdateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr // Optional
    };

    VkDescriptorSetLayoutBinding samplerLayoutBinding {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr
    };

    array<VkDescriptorSetLayoutBinding, 2> layoutBindings {
        uniformBufferLayoutBinding,
        samplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
        .pBindings = layoutBindings.data()
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &layoutInfo,
        nullptr,
        &descriptorSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::createGraphicsPipeline()
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
        &pipeline));
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::createCommandBuffers()
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
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSets[i]);
        commandBuffer->bindVertexBuffers(vertexBuffers);
        commandBuffer->bindIndexBuffer(indexBuffer);
        commandBuffer->drawIndexed(indexBuffer->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::update(float deltaTime)
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    ubo.model = glm::identity<mat4>();
    ubo.view = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = perspective(radians(45.0f), swapChainDesc.extent.width / (float) swapChainDesc.extent.height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;

    void* mappedMemory = nullptr;
    graphicsDevice->map(uniformBuffers[currentImageIndex], &mappedMemory);
    memcpy(mappedMemory, &ubo, sizeof(ubo));
    graphicsDevice->unmap(uniformBuffers[currentImageIndex]);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::cleanup()
{
    vkDestroyDescriptorSetLayout(graphicsDevice->getLogicalDevice(), descriptorSetLayout, nullptr);

    vertexBuffer.reset();
    indexBuffer.reset();
    vertexShader.reset();
    fragmentShader.reset();
    texture.reset();

    Application::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::cleanupSwapChain()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

    uniformBuffers.clear();

    Application::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::recreateSwapChain()
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

void TexturedQuadTest::createTexture()
{
//    const path texturePath =
//        getAssetsPath() / "samples/vulkan_asset_pack_gltf/data/textures/lavaplanet_rgba.ktx";
    const path texturePath =
        getAssetsDirectory() / "textures/ttex_v01/metal08.jpg";

    TextureLoader textureLoader(graphicsDevice);
    texture = textureLoader.loadTexture2D(texturePath);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedQuadTest::updateDevTools()
{
    devTools->sliderFloat(
        "LOD bias",
        &ubo.lodBias,
        0.0f,
        static_cast<float>(texture->getImage()->getDesc().mipLevels));
}

// ---------------------------------------------------------------------------------------------------------------------
