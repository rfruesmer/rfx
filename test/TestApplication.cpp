#include "rfx/pch.h"
#include "TestApplication.h"

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;


// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initGraphicsResources()
{
    createSceneResources();
    createMeshResources();

    createRenderPass();
    createPipelines();
    createFrameBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createDescriptorPool()
{
    const uint32_t uniformBufferDescCount = 2000;
    const uint32_t combinedImageSamplerDescCount = 2000;
    const uint32_t maxSets = 2000;

    vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBufferDescCount },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, combinedImageSamplerDescCount }
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
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

void TestApplication::createSceneResources()
{
    createSceneDataBuffer();
    createSceneDescriptorSetLayout();
    createSceneDescriptorSet();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createSceneDataBuffer()
{
    sceneDataBuffer_ = graphicsDevice->createBuffer(
        sizeof(SceneData),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice->bind(sceneDataBuffer_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createSceneDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    const VkDescriptorSetLayoutCreateInfo sceneDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &sceneDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &sceneDescSetLayoutCreateInfo,
        nullptr,
        &sceneDescriptorSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createSceneDescriptorSet()
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &sceneDescriptorSetLayout_
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        &allocInfo,
        &sceneDescriptorSet_));

    const VkWriteDescriptorSet writeDescriptorSet =
        buildWriteDescriptorSet(sceneDescriptorSet_, 0, &sceneDataBuffer_->getDescriptorBufferInfo());

    vkUpdateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createMeshDataBuffers(const ModelPtr& model)
{
    for (const auto& node : model->getGeometryNodes()) {
        if (node->getMeshCount() == 0) {
            continue;
        }

        RFX_CHECK_STATE(node->getMeshCount() == 1, "");

        BufferPtr meshDataBuffer = graphicsDevice->createBuffer(
            sizeof(MeshData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice->bind(meshDataBuffer);
        meshDataBuffer->load(sizeof(mat4), &node->getWorldTransform());

        node->getMeshes().at(0)->setDataBuffer(meshDataBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createMeshDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    const VkDescriptorSetLayoutCreateInfo meshDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &meshDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &meshDescSetLayoutCreateInfo,
        nullptr,
        &meshDescriptorSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createMeshDescriptorSets(const ModelPtr& model)
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &meshDescriptorSetLayout_
    };

    for (const auto& mesh : model->getMeshes()) {

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            &allocInfo,
            &descriptorSet));

        const VkWriteDescriptorSet writeDescriptorSet =
            buildWriteDescriptorSet(
                descriptorSet,
                0,
                &mesh->getDataBuffer()->getDescriptorBufferInfo());

        vkUpdateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);

        mesh->setDescriptorSet(descriptorSet);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createRenderPass()
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

    VkSubpassDescription subpassDescription {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pResolveAttachments = multiSampleCount > VK_SAMPLE_COUNT_1_BIT ? &colorAttachmentResolveRef : nullptr,
        .pDepthStencilAttachment = depthBuffer ? &depthAttachmentRef : nullptr
    };

    vector<VkSubpassDependency> subpassDependencies {
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        },
        {
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        }
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
        .pSubpasses = &subpassDescription,
        .dependencyCount = static_cast<uint32_t>(subpassDependencies.size()),
        .pDependencies = subpassDependencies.data()
    };

    ThrowIfFailed(vkCreateRenderPass(
        graphicsDevice->getLogicalDevice(),
        &renderPassCreateInfo,
        nullptr,
        &renderPass));
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineLayout TestApplication::createDefaultPipelineLayout(const vector<VkDescriptorSetLayout>& descriptorSetLayouts)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreatePipelineLayout(
        graphicsDevice->getLogicalDevice(),
        &pipelineLayoutInfo,
        nullptr,
        &pipelineLayout));

    return pipelineLayout;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline TestApplication::createDefaultPipelineFor(
    const MaterialShaderPtr& shader,
    VkPipelineLayout pipelineLayout)
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
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

    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = graphicsDevice->getMultiSampleCount(),
        .sampleShadingEnable = VK_FALSE
    };

    const vector<VkDynamicState> dynamicStates {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    const array<VkPipelineShaderStageCreateInfo, 2> shaderStages {
        shader->getVertexShader()->getStageCreateInfo(),
        shader->getFragmentShader()->getStageCreateInfo()
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shaderStages.size(),
        .pStages = shaderStages.data(),
        .pVertexInputState = &shader->getVertexShader()->getVertexInputStateCreateInfo(),
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = &depthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1 // Optional
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateGraphicsPipelines(
        graphicsDevice->getLogicalDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineCreateInfo,
        nullptr,
        &pipeline));


    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
    ThrowIfFailed(vkCreateGraphicsPipelines(
        graphicsDevice->getLogicalDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineCreateInfo,
        nullptr,
        &wireframePipeline));

    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::beginMainLoop()
{
    Application::beginMainLoop();

    lockMouseCursor();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::lockMouseCursor(bool lock)
{
    double x, y;
    glfwGetCursorPos(window_->getGlfwWindow(), &x, &y);
    lastMousePos = { x, y };

    glfwSetInputMode(
        window_->getGlfwWindow(),
        GLFW_CURSOR,
        lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    mouseCursorLocked = lock;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::update(float deltaTime)
{
    Application::update(deltaTime);

    updateCamera(deltaTime);
    updateSceneData(deltaTime);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateCamera(float deltaTime)
{
    const float movementSpeed = 0.005f;
    GLFWwindow* glfwWindow = window_->getGlfwWindow();

    vec3 velocity {};

    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
        velocity.x = -movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
        velocity.x = movementSpeed;
    }

    if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        velocity.y = movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        velocity.y = -movementSpeed;
    }

    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
        velocity.z = -movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
        velocity.z = movementSpeed;
    }

    camera.setVelocity(velocity);

    if (mouseCursorLocked) {
        double x, y;
        glfwGetCursorPos(window_->getGlfwWindow(), &x, &y);

        vec2 offset = {
            x - lastMousePos.x,
            y - lastMousePos.y
        };
        lastMousePos = { x, y };

        const float sensitivity = 0.1f;
        offset *= sensitivity;

        camera.addYaw(offset.x);
        camera.addPitch(offset.y);
    }

    camera.update(deltaTime);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateDevTools()
{
    if (devTools->checkBox("Wireframe", &wireframe)) {
        createCommandBuffers();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::cleanup()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    renderGraph->clear();

    if (wireframePipeline) {
        vkDestroyPipeline(device, wireframePipeline, nullptr);
        wireframePipeline = VK_NULL_HANDLE;
    }

    Application::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::cleanupSwapChain()
{
    VkDevice device = graphicsDevice->getLogicalDevice();

    vkDestroyDescriptorSetLayout(device, sceneDescriptorSetLayout_, nullptr);
    sceneDataBuffer_.reset();

    vkDestroyDescriptorSetLayout(device, meshDescriptorSetLayout_, nullptr);

    Application::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::recreateSwapChain()
{
    Application::recreateSwapChain();

    createSceneResources();
    createMeshResources();

    createRenderPass();
    createFrameBuffers();
    createCommandBuffers();

    updateProjection();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::onKeyEvent(
    const Window& window,
    int key,
    int scancode,
    int action,
    int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window.getGlfwWindow(), GLFW_TRUE);
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        lockMouseCursor(true);
    }
    else if (mods & GLFW_MOD_CONTROL
             && mods & GLFW_MOD_ALT
             && action == GLFW_PRESS) {
        lockMouseCursor(false);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

mat4 TestApplication::calcDefaultProjection()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    mat4 proj = perspective(
        radians(45.0f),
        static_cast<float>(swapChainDesc.extent.width) / static_cast<float>(swapChainDesc.extent.height),
        0.1f,
        1000.0f);
    proj[1][1] *= -1;

    return proj;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::setProjectionMatrix(const glm::mat4& projection)
{
    sceneData_.projMatrix = projection;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::setViewMatrix(const mat4& viewMatrix)
{
    sceneData_.viewMatrix = viewMatrix;
    sceneData_.lightPos = viewMatrix * vec4(light_.getPosition(), 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::setLight(const PointLight& light)
{
    light_ = light;

    sceneData_.lightPos = sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f);
    sceneData_.La = vec4(0.01f, 0.01f, 0.01f, 1.0f);
    sceneData_.Ld = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    sceneData_.Ls = vec4(0.3f, 0.3f, 0.3f, 1.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initMaterialUniformBuffer(
    const MaterialPtr& material,
    const MaterialShaderPtr& shader)
{
    const vector<std::byte> materialData = shader->createDataFor(material);
    const BufferPtr materialDataBuffer = createAndBindUniformBuffer(materialData.size());
    materialDataBuffer->load(materialData.size(), materialData.data());
    material->setUniformBuffer(materialDataBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

BufferPtr TestApplication::createAndBindUniformBuffer(VkDeviceSize bufferSize)
{
    shared_ptr<Buffer> uniformBuffer = graphicsDevice->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice->bind(uniformBuffer);

    return uniformBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initMaterialDescriptorSetLayout(
    const MaterialPtr& material,
    const MaterialShaderPtr& shader)
{
    VkDescriptorSet materialDescriptorSet = createMaterialDescriptorSetFor(
        material,
        shader->getMaterialDescriptorSetLayout());
    material->setDescriptorSet(materialDescriptorSet);
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet TestApplication::createMaterialDescriptorSetFor(
    const MaterialPtr& material,
    VkDescriptorSetLayout descriptorSetLayout)
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        &allocInfo,
        &descriptorSet));

    vector<VkWriteDescriptorSet> writeDescriptorSets;
    uint32_t binding = 0;

    writeDescriptorSets.push_back(
        buildWriteDescriptorSet(
            descriptorSet,
            binding++,
            &material->getUniformBuffer()->getDescriptorBufferInfo()));

    if (material->getBaseColorTexture() != nullptr) {
        writeDescriptorSets.push_back(
            buildWriteDescriptorSet(
                descriptorSet,
                binding++,
                &material->getBaseColorTexture()->getDescriptorImageInfo()));
    }

    if (material->getNormalTexture() != nullptr) {
        writeDescriptorSets.push_back(
            buildWriteDescriptorSet(
                descriptorSet,
                binding++,
                &material->getNormalTexture()->getDescriptorImageInfo()));
    }

    if (material->getMetallicRoughnessTexture() != nullptr) {
        writeDescriptorSets.push_back(
            buildWriteDescriptorSet(
                descriptorSet,
                binding++,
                &material->getMetallicRoughnessTexture()->getDescriptorImageInfo()));
    }

    if (material->getOcclusionTexture() != nullptr) {
        writeDescriptorSets.push_back(
            buildWriteDescriptorSet(
                descriptorSet,
                binding++,
                &material->getOcclusionTexture()->getDescriptorImageInfo()));
    }

    if (material->getEmissiveTexture() != nullptr) {
        writeDescriptorSets.push_back(
            buildWriteDescriptorSet(
                descriptorSet,
                binding++,
                &material->getEmissiveTexture()->getDescriptorImageInfo()));
    }

    vkUpdateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        writeDescriptorSets.size(),
        writeDescriptorSets.data(),
        0,
        nullptr);

    return descriptorSet;
}

// ---------------------------------------------------------------------------------------------------------------------

VkWriteDescriptorSet TestApplication::buildWriteDescriptorSet(
    VkDescriptorSet descriptorSet,
    uint32_t binding,
    const VkDescriptorImageInfo* descriptorImageInfo)
{
    return {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = descriptorImageInfo
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkWriteDescriptorSet TestApplication::buildWriteDescriptorSet(
    VkDescriptorSet descriptorSet,
    uint32_t binding,
    const VkDescriptorBufferInfo* descriptorBufferInfo)
{
    return {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = descriptorBufferInfo
    };
}

// ---------------------------------------------------------------------------------------------------------------------
