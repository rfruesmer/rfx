#include "rfx/pch.h"
#include "CubeMapTest.h"
#include "SkyBoxShader.h"
#include "ReflectionShader.h"
#include "rfx/application/ModelLoader.h"
#include "rfx/application/TextureLoader.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/PipelineUtil.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<CubeMapTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createDescriptorPool();
//    createShadersFor(skyBoxModel, SkyBoxShader::ID);
    updateProjection();

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::loadScene()
{
    const path skyBoxModelPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/models/cube.gltf";
    const path skyBoxCubeMapPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/textures/cubemap_yokohama_rgba.ktx";
    const path skyBoxVertexShaderPath = getAssetsDirectory() / "shaders/skybox.vert";
    const path skyBoxFragmentShaderPath = getAssetsDirectory() / "shaders/skybox.frag";


    ModelLoader modelLoader(graphicsDevice);
    skyBoxModel = modelLoader.load(skyBoxModelPath);

    TextureLoader textureLoader(graphicsDevice);
    skyBoxCubeMap = textureLoader.loadCubeMap(skyBoxCubeMapPath);

    skyBoxShaderProgram =
        ShaderLoader(graphicsDevice).loadProgram(
            skyBoxVertexShaderPath,
            skyBoxFragmentShaderPath,
            skyBoxModel->getVertexBuffer()->getVertexFormat());

    camera->setPosition({ 0.0f, 0.0f, 0.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
//    shaderFactory.addAllocator(SkyBoxShader::ID,
//        [this] { return make_shared<SkyBoxShader>(graphicsDevice); });
//
//    shaderFactory.addAllocator(ReflectionShader::ID,
//        [this] { return make_shared<ReflectionShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateShaderData()
{
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createMeshResources()
{
    TestApplication::createMeshResources();

//    createMeshDataBuffers(skyBoxModel);
//    createMeshDescriptorSets(skyBoxModel);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
//    renderGraph->add(skyBoxModel, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::cleanup()
{
    skyBoxShader.reset();
    skyBoxShaderProgram.reset();
    skyBoxModel.reset();
    skyBoxDataBuffer.reset();
    skyBoxCubeMap.reset();
    vkDestroyPipeline(graphicsDevice->getLogicalDevice(), skyBoxPipeline, nullptr);
    vkDestroyPipelineLayout(graphicsDevice->getLogicalDevice(), skyBoxPipelineLayout, nullptr);

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::cleanupSwapChain()
{
    vkDestroyDescriptorSetLayout(graphicsDevice->getLogicalDevice(), skyBoxDescriptorSetLayout, nullptr);

    TestApplication::cleanupSwapChain();
}


// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateDevTools()
{
    TestApplication::updateDevTools();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSceneResources()
{
    TestApplication::createSceneResources();

    createSkyboxResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSkyboxResources()
{
    createSkyboxDataBuffer();
    createSkyboxDescriptorSetLayout();
    createSkyboxDescriptorSet();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSkyboxDataBuffer()
{
    skyBoxDataBuffer = graphicsDevice->createBuffer(
        sizeof(SkyBoxData),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice->bind(skyBoxDataBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSkyboxDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };

    const VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(descSetLayoutBindings.size()),
        .pBindings = descSetLayoutBindings.data()
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice->getLogicalDevice(),
        &descSetLayoutCreateInfo,
        nullptr,
        &skyBoxDescriptorSetLayout));
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSkyboxDescriptorSet()
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &skyBoxDescriptorSetLayout
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        &allocInfo,
        &skyBoxDescriptorSet));

    vector<VkWriteDescriptorSet> writeDescriptorSets {
        buildWriteDescriptorSet(
            skyBoxDescriptorSet,
            0,
            &skyBoxDataBuffer->getDescriptorBufferInfo()),
        buildWriteDescriptorSet(
            skyBoxDescriptorSet,
            1,
            &skyBoxCubeMap->getDescriptorImageInfo())
    };

    vkUpdateDescriptorSets(
        graphicsDevice->getLogicalDevice(),
        writeDescriptorSets.size(),
        writeDescriptorSets.data(),
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createPipelines()
{
    skyBoxPipelineLayout = createPipelineLayout({skyBoxDescriptorSetLayout});

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        PipelineUtil::getDefaultRasterizationState();
    rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;

    const VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
        PipelineUtil::getDefaultColorBlendAttachmentState();

    const vector<VkDynamicState> dynamicStates {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };


    skyBoxPipeline = createPipeline(
        skyBoxPipelineLayout,
        PipelineUtil::getDefaultInputAssemblyState(),
        rasterizationState,
        PipelineUtil::getDefaultColorBlendState(&colorBlendAttachmentState),
        PipelineUtil::getDefaultDepthStencilState(),
        PipelineUtil::getDefaultViewportState(),
        PipelineUtil::getDefaultMultisampleState(graphicsDevice->getMultiSampleCount()),
        PipelineUtil::getDynamicState(dynamicStates),
        skyBoxShaderProgram);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateSceneData()
{
    skyBoxData.projMatrix = sceneData_.projMatrix;
    skyBoxData.modelMatrix = camera->getViewMatrix();
    skyBoxData.modelMatrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    skyBoxDataBuffer->load(sizeof(SkyBoxData), &skyBoxData);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createCommandBuffers()
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

    const vector<shared_ptr<VertexBuffer>> vertexBuffers = { skyBoxModel->getVertexBuffer() };

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
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxPipeline);
        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxPipelineLayout, 0, skyBoxDescriptorSet);
        commandBuffer->bindVertexBuffers(vertexBuffers);
        commandBuffer->bindIndexBuffer(skyBoxModel->getIndexBuffer());
        commandBuffer->drawIndexed(skyBoxModel->getIndexBuffer()->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
