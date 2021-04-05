#include "rfx/pch.h"
#include "CubeMapTest.h"
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
    TestApplication::initGraphics();

    loadScene();

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::loadScene()
{
    camera->setPosition({ 0.0f, 0.0f, 0.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateShaderData()
{
    skyBox->updateUniformBuffer(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSceneResources()
{
    TestApplication::createSceneResources();


    const path skyBoxModelPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/models/cube.gltf";
    const path skyBoxCubeMapPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/textures/cubemap_yokohama_rgba.ktx";
    const path skyBoxVertexShaderPath = getAssetsDirectory() / "shaders/skybox.vert";
    const path skyBoxFragmentShaderPath = getAssetsDirectory() / "shaders/skybox.frag";

    skyBox = make_shared<SkyBox>(
        graphicsDevice,
        descriptorPool);

    skyBox->create(
        skyBoxModelPath,
        skyBoxCubeMapPath,
        skyBoxVertexShaderPath,
        skyBoxFragmentShaderPath,
        renderPass);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
//    renderGraph->add(skyBoxModel, materialShaderMap);
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

    const ModelPtr& skyBoxModel = skyBox->getModel();
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
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, skyBox->getPipeline());
        commandBuffer->bindDescriptorSet(
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            skyBox->getPipelineLayout(),
            0,
            skyBox->getDescriptorSet());
        commandBuffer->bindVertexBuffers(vertexBuffers);
        commandBuffer->bindIndexBuffer(skyBoxModel->getIndexBuffer());
        commandBuffer->drawIndexed(skyBoxModel->getIndexBuffer()->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------


void CubeMapTest::cleanup()
{
    skyBox.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::cleanupSwapChain()
{
    if (skyBox != nullptr) {
        skyBox->cleanupSwapChain();
    }

    TestApplication::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------
