#include "rfx/pch.h"
#include "TestApplication.h"
#include "rfx/application/ShaderLoader.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;


// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::loadShaders()
{
    const path shadersDirectory = getAssetsDirectory() / "shaders";
    const path vertexShaderPath = shadersDirectory / effect->getVertexShaderFileName();
    const path fragmentShaderPath = shadersDirectory / effect->getFragmentShaderFileName();

    const ShaderLoader shaderLoader(graphicsDevice);
    vertexShader = shaderLoader.loadVertexShader(
        vertexShaderPath,
        "main",
        effect->getVertexFormat());
    fragmentShader = shaderLoader.loadFragmentShader(
        fragmentShaderPath,
        "main");
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::initGraphicsResources()
{
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSetLayouts();
    createDescriptorSets();
    createRenderPass();
    createPipelineLayout();
    createPipeline();
    createFrameBuffers();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createUniformBuffers()
{
    effect->createUniformBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createDescriptorPool()
{
    effect->createDescriptorPool();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createDescriptorSetLayouts()
{
    effect->createDescriptorSetLayouts();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createDescriptorSets()
{
    effect->createDescriptorSets();
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
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
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

void TestApplication::createPipelineLayout()
{
    vector<VkDescriptorSetLayout> descriptorSetLayouts = effect->getDescriptorSetLayouts();

    VkPushConstantRange pushConstantRange {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(mat4)
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange
    };

    ThrowIfFailed(vkCreatePipelineLayout(
        graphicsDevice->getLogicalDevice(),
        &pipelineLayoutInfo,
        nullptr,
        &pipelineLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::createPipeline()
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

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = graphicsDevice->getMultiSampleCount(),
        .sampleShadingEnable = VK_FALSE
    };

    const vector<VkDynamicState> dynamicStateEnables {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size()),
        .pDynamicStates = dynamicStateEnables.data()
    };

    const array<VkPipelineShaderStageCreateInfo, 2> shaderStages {
        vertexShader->getStageCreateInfo(),
        fragmentShader->getStageCreateInfo()
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shaderStages.size(),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexShader->getVertexInputStateCreateInfo(),
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = &depthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo, // Optional
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
        &pipelineCreateInfo,
        nullptr,
        &defaultPipeline));


    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
    ThrowIfFailed(vkCreateGraphicsPipelines(
        graphicsDevice->getLogicalDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineCreateInfo,
        nullptr,
        &wireframePipeline));
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
    updateSceneData();
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
    effect->cleanupSwapChain();
    effect.reset();
    scene.reset();
    vertexShader.reset();
    fragmentShader.reset();

    Application::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::cleanupSwapChain()
{
    if (effect) {
        effect->cleanupSwapChain();
    }

    vkDestroyPipeline(graphicsDevice->getLogicalDevice(), wireframePipeline, nullptr);

    Application::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestApplication::recreateSwapChain()
{
    Application::recreateSwapChain();

    initGraphicsResources();
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
