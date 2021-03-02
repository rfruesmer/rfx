#include "rfx/pch.h"
#include "SceneTest.h"
#include "rfx/application/SceneLoader.h"
#include "rfx/application/ShaderLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<SceneTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

SceneTest::SceneTest()
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    loadShaders();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::loadScene()
{
    const path assetsPath = getAssetsDirectory();getAssetsDirectory();
//    const path scenePath = assetsPath / "samples/vulkan_asset_pack_gltf/data/models/FlightHelmet/glTF/FlightHelmet.gltf";
//    const path scenePath = assetsPath / "models/quad/quad.gltf";
    const path scenePath = assetsPath / "models/sphere/sphere.gltf";

    SceneLoader sceneLoader(graphicsDevice);
    scene = sceneLoader.load(scenePath, vertexFormat);

    camera.setPosition(vec3(0.0f, 1.0f, 2.0f));
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::loadShaders()
{
    const path assetsDirectory = getAssetsDirectory();
//    const path vertexShaderPath = assetsDirectory / "shaders/default.vert";
//    const path fragmentShaderPath = assetsDirectory / "shaders/default.frag";
    const path vertexShaderPath = assetsDirectory / "shaders/diffuse.vert";
    const path fragmentShaderPath = assetsDirectory / "shaders/diffuse.frag";

    const ShaderLoader shaderLoader(graphicsDevice);
    vertexShader = shaderLoader.loadVertexShader(
        vertexShaderPath,
        "main",
        vertexFormat);
    fragmentShader = shaderLoader.loadFragmentShader(fragmentShaderPath, "main");
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::initGraphicsResources()
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

void SceneTest::createUniformBuffers()
{
    sceneUniformBuffer = graphicsDevice->createBuffer(
        sizeof(UniformBufferObject),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice->bind(sceneUniformBuffer);

    for (const auto& mesh : scene->getMeshes()) {
        shared_ptr<Buffer> meshUniformBuffer = graphicsDevice->createBuffer(
            sizeof(UniformBufferObject),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice->bind(meshUniformBuffer);

        void* mappedMemory = nullptr;
        graphicsDevice->map(meshUniformBuffer, &mappedMemory);
        memcpy(mappedMemory, &mesh->getWorldTransform(), sizeof(mat4));
        graphicsDevice->unmap(meshUniformBuffer);

        meshUniformBuffers.push_back(meshUniformBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::createDescriptorPool()
{
    const uint32_t meshCount = scene->getMeshCount();

    vector<VkDescriptorPoolSize> poolSizes {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1 + meshCount
        }
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1 + meshCount,
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

void SceneTest::createDescriptorSetLayouts()
{
    VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo sceneDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &sceneDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &sceneDescSetLayoutCreateInfo,
        nullptr,
        &sceneDescSetLayout));

    // ---

    VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo meshDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &meshDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &meshDescSetLayoutCreateInfo,
        nullptr,
        &meshDescSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::createDescriptorSets()
{
    {
        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &sceneDescSetLayout
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            &allocInfo,
            &sceneDescSet));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = sceneUniformBuffer->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sceneDescSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        vkUpdateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }

    meshDescSets.resize(scene->getMeshCount());

    for (uint32_t i = 0, count = scene->getMeshCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &meshDescSetLayout
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            &allocInfo,
            &meshDescSets[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = meshUniformBuffers[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = meshDescSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        vkUpdateDescriptorSets(
            graphicsDevice->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::createRenderPass()
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

void SceneTest::createPipelineLayout()
{
    vector<VkDescriptorSetLayout> descriptorSetLayouts = {
        sceneDescSetLayout,
        meshDescSetLayout
    };

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

void SceneTest::createPipeline()
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

void SceneTest::createCommandBuffers()
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

    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();
    commandBuffers = graphicsDevice->createCommandBuffers(graphicsCommandPool, swapChainFramebuffers.size());

    for (size_t i = 0; i < commandBuffers.size(); ++i) {

        renderPassBeginInfo.framebuffer = swapChainFramebuffers[i];

        const auto& commandBuffer = commandBuffers[i];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPassBeginInfo);
        commandBuffer->setViewport(viewport);
        commandBuffer->setScissor(scissor);
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, wireframe ? wireframePipeline : defaultPipeline);
//        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, sceneDescSet);
        commandBuffer->bindVertexBuffer(scene->getVertexBuffer());
        commandBuffer->bindIndexBuffer(scene->getIndexBuffer());

        drawScene(commandBuffer);

        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::drawScene(const shared_ptr<CommandBuffer>& commandBuffer)
{
    for (size_t i = 0, count = scene->getMeshes().size(); i < count; ++i) {

        const vector<VkDescriptorSet> descriptorSets {
            sceneDescSet,
            meshDescSets[i]
        };

        commandBuffer->bindDescriptorSets(
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            descriptorSets);


        const auto& mesh = scene->getMesh(i);
        for (const auto& subMesh : mesh->getSubMeshes()) {

            if (subMesh.indexCount == 0) {
                continue;
            }

            const shared_ptr<Material>& material = scene->getMaterial(subMesh.materialIndex);
            PushConstant pushConstant {
                .Ld = { 1.0f, 1.0f, 1.0f, 1.0f },
                .Kd = material->getBaseColorFactor()
            };

            commandBuffer->pushConstants(
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(PushConstant),
                &pushConstant);

            if (vertexFormat.containsTexCoords()) {
                const shared_ptr<Texture2D>& baseColorTexture = material->getBaseColorTexture();

                commandBuffer->bindDescriptorSet(
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineLayout,
                    1,
                    *baseColorTexture->getSamplerDescriptorSet());
            }

            commandBuffer->drawIndexed(subMesh.indexCount, subMesh.firstIndex);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::beginMainLoop()
{
    Application::beginMainLoop();

    lockMouseCursor();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::update(float deltaTime)
{
    Application::update(deltaTime);

    updateCamera(deltaTime);
    updateUniformBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::updateCamera(float deltaTime)
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

void SceneTest::updateUniformBuffer()
{
    const SwapChainDesc& swapChainDesc = graphicsDevice->getSwapChain()->getDesc();

    mat4 proj = perspective(
        radians(45.0f),
        static_cast<float>(swapChainDesc.extent.width) / static_cast<float>(swapChainDesc.extent.height),
        0.1f,
        1000.0f);
    proj[1][1] *= -1;

    sceneUBO.viewMatrix = camera.getViewMatrix();
    sceneUBO.projMatrix = proj;
    sceneUBO.lightPos = sceneUBO.viewMatrix * vec4(5.0f, 5.0f, 2.0f, 1.0f); // TODO: move to light class

    void* mappedMemory = nullptr;
    graphicsDevice->map(sceneUniformBuffer, &mappedMemory);
    memcpy(mappedMemory, &sceneUBO, sizeof(sceneUBO));
    graphicsDevice->unmap(sceneUniformBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::updateDevTools()
{
    if (devTools->checkBox("Wireframe", &wireframe)) {
        createCommandBuffers();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::cleanup()
{
    sceneUniformBuffer.reset();
    for (auto& meshUniformBuffer : meshUniformBuffers) {
        meshUniformBuffer.reset();
    }

    scene.reset();
    vertexShader.reset();
    fragmentShader.reset();

    Application::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::cleanupSwapChain()
{
    vkDestroyDescriptorSetLayout(graphicsDevice->getLogicalDevice(), sceneDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(graphicsDevice->getLogicalDevice(), meshDescSetLayout, nullptr);
    vkDestroyPipeline(graphicsDevice->getLogicalDevice(), wireframePipeline, nullptr);

    Application::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::recreateSwapChain()
{
    Application::recreateSwapChain();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void SceneTest::onKeyEvent(
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

void SceneTest::lockMouseCursor(bool lock)
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

