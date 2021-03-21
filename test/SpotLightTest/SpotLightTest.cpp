#include "rfx/pch.h"
#include "SpotLightTest.h"
#include "rfx/application/ModelLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<SpotLightTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

SpotLightTest::SpotLightTest()
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createEffects();
    updateProjection();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/plane/plane_with_spotlight.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(
        scenePath,
        SpotLightEffect::VERTEX_SHADER_ID,
        SpotLightEffect::FRAGMENT_SHADER_ID);

//    for (const auto& material : scene->getMaterials()) {
//        material->setSpecularFactor({1.0f, 0.0f, 0.0f});
//    }
    camera.setPosition({0.0f, 1.0f, 2.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createEffects()
{
    const path shadersDirectory = getAssetsDirectory() / "shaders";

    // TODO: support for multiple/different materials/effects/shaders per scene
    const shared_ptr<Material>& material = scene->getMaterial(0);

    RFX_CHECK_STATE(scene->getLightCount() > 0, "");
    auto light = dynamic_pointer_cast<SpotLight>(scene->getLight(0));
    RFX_CHECK_STATE(light != nullptr, "");

    effect = make_unique<SpotLightEffect>(graphicsDevice, scene);
    effect->loadShaders(material, shadersDirectory);
    effect->setLight(light);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createUniformBuffers()
{
    effect->createUniformBuffers();

    const VkDeviceSize bufferSize = sizeof(SpotLightEffect::MaterialData);

    for (const auto& material : scene->getMaterials())
    {
        const SpotLightEffect::MaterialData materialData {
            .baseColor = material->getBaseColorFactor(),
            .specular = material->getSpecularFactor(),
            .shininess = material->getShininess()
        };

        material->setUniformBuffer(
            createAndBindUniformBuffer(bufferSize, &materialData));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createDescriptorPools()
{
    effect->createDescriptorPools();
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createDescriptorSetLayouts()
{
    effect->createDescriptorSetLayouts();

    for (const auto& material : scene->getMaterials()) {
        material->createDescriptorSetLayout();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createDescriptorSets()
{
    effect->createDescriptorSets();

    for (const auto& material : scene->getMaterials()) {
        material->createDescriptorSet(effect->getDescriptorPool());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createPipelineLayouts()
{
    vector<VkDescriptorSetLayout> descriptorSetLayouts = effect->getDescriptorSetLayouts();
    descriptorSetLayouts.push_back(scene->getMaterial(0)->getDescriptorSetLayout());

    TestApplication::createDefaultPipelineLayout(descriptorSetLayouts);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createPipelines()
{
    TestApplication::createDefaultPipeline(*effect);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createCommandBuffers()
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
        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, effect->getSceneDescriptorSet());
        commandBuffer->bindVertexBuffer(scene->getVertexBuffer());
        commandBuffer->bindIndexBuffer(scene->getIndexBuffer());

        drawScene(commandBuffer);

        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::drawScene(const shared_ptr<CommandBuffer>& commandBuffer)
{
    const vector<VkDescriptorSet>& meshDescSets = effect->getMeshDescriptorSets();


    for (size_t i = 0, count = scene->getMeshes().size(); i < count; ++i) {

        commandBuffer->bindDescriptorSet(
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            1,
            meshDescSets[i]);

        const auto& mesh = scene->getMesh(i);
        for (const auto& subMesh : mesh->getSubMeshes()) {

            if (subMesh.indexCount == 0) {
                continue;
            }

            commandBuffer->bindDescriptorSet(
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                2,
                subMesh.material->getDescriptorSet());

            commandBuffer->drawIndexed(subMesh.indexCount, subMesh.firstIndex);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::updateProjection()
{
    effect->setProjectionMatrix(calcDefaultProjection());
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::updateSceneData(float deltaTime)
{
    effect->setViewMatrix(camera.getViewMatrix());
    effect->updateSceneDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::cleanup()
{
    effect->cleanupSwapChain();
    effect.reset();

    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::cleanupSwapChain()
{
    if (effect) {
        effect->cleanupSwapChain();
    }

    if (scene) {
        for (const auto& material : scene->getMaterials()) {
            material->destroyDescriptorSetLayout();
        }
    }

    TestApplication::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

