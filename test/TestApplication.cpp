#include "rfx/pch.h"
#include "TestApplication.h"
#include "rfx/graphics/PipelineUtil.h"

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initGraphics()
{
    Application::initGraphics();

    createDescriptorPool();
    updateProjection();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initGraphicsResources()
{
    createRenderPass();

    createSceneResources();
    createMeshResources();

    createPipelines();
    createFrameBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createDescriptorPool()
{
    const uint32_t uniformBufferDescCount = 8000;
    const uint32_t combinedImageSamplerDescCount = 8000;
    const uint32_t maxSets = 8000;

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

void TestApplication::createMeshResources()
{
    createMeshDescriptorSetLayout();
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

void TestApplication::createPipelines()
{
    for (const auto& [shader, materials] : materialShaderMap)
    {
        vector<VkDescriptorSetLayout> descriptorSetLayouts {
            sceneDescriptorSetLayout_,
            shader->getShaderDescriptorSetLayout(),
            shader->getMaterialDescriptorSetLayout(),
            meshDescriptorSetLayout_
        };

        VkPipelineLayout pipelineLayout =
            PipelineUtil::createPipelineLayout(
                graphicsDevice,
                descriptorSetLayouts);
        VkPipeline pipeline = createPipelineFor(shader->getShaderProgram(), pipelineLayout);
        shader->setPipeline(pipelineLayout, pipeline);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline TestApplication::createPipelineFor(
    const ShaderProgramPtr& shaderProgram,
    VkPipelineLayout pipelineLayout)
{
    const VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
        PipelineUtil::getDefaultColorBlendAttachmentState();

    const vector<VkDynamicState> dynamicStates {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    return PipelineUtil::createPipeline(
        graphicsDevice,
        pipelineLayout,
        PipelineUtil::getDefaultInputAssemblyState(),
        PipelineUtil::getDefaultRasterizationState(),
        PipelineUtil::getDefaultColorBlendState(&colorBlendAttachmentState),
        PipelineUtil::getDefaultDepthStencilState(),
        PipelineUtil::getDefaultViewportState(),
        PipelineUtil::getDefaultMultisampleState(graphicsDevice->getMultiSampleCount()),
        PipelineUtil::getDynamicState(dynamicStates),
        shaderProgram,
        renderPass);
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

    camera->setVelocity(velocity);

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

        camera->addYaw(offset.x);
        camera->addPitch(offset.y);
    }

    camera->update(deltaTime);
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

    for (const auto& [shader, materials] : materialShaderMap) {
        shader->destroy();
    }
    materialShaderMap.clear();

    renderGraph.reset();

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

    createRenderPass();

    createSceneResources();
    createMeshResources();

    createFrameBuffers();
    buildRenderGraph();
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

void TestApplication::updateProjection()
{
    camera->setProjectionMatrix(calcDefaultProjection());
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

void TestApplication::updateSceneData(float deltaTime)
{
    updateSceneData();
    updateSceneDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateSceneDataBuffer()
{
    sceneDataBuffer_->load(sizeof(SceneData), &sceneData_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createShadersFor(
    const ModelPtr& model,
    const string& defaultShaderId)
{
    MaterialShaderFactory shaderFactory(
        graphicsDevice,
        descriptorPool,
        getShadersDirectory(),
        defaultShaderId);

    initShaderFactory(shaderFactory);

    for (const auto& material : model->getMaterials())
    {
        const MaterialShaderPtr shader = shaderFactory.createShaderFor(material);
        initMaterialUniformBuffer(material, shader);
        initMaterialDescriptorSet(material, shader);

        materialShaderMap[shader].push_back(material);
    }

    updateShaderData();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::updateSceneData()
{
    sceneData_.projMatrix = camera->getProjectionMatrix();
    sceneData_.viewMatrix = camera->getViewMatrix();
    sceneData_.cameraPosition = camera->getPosition();

    updateShaderData(); // TODO: move out from here
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

void TestApplication::initMaterialDescriptorSet(
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

void TestApplication::createCommandBuffers()
{
    RFX_CHECK_STATE(renderGraph != nullptr, "");

    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const vector<VkFramebuffer>& swapChainFrameBuffers = swapChain->getFramebuffers();

    commandBuffers = graphicsDevice->createCommandBuffers(
        graphicsDevice->getGraphicsCommandPool(),
        swapChainFrameBuffers.size());

    for (size_t i = 0; i < commandBuffers.size(); ++i)
    {
        const auto& commandBuffer = commandBuffers[i];

        renderGraph->record(
            commandBuffer,
            renderPass,
            swapChainFrameBuffers[i]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
