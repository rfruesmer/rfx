#include "rfx/pch.h"
#include "NormalMapTest.h"
#include "rfx/application/ModelLoader.h"
#include "rfx/common/Logger.h"
#include <cmath>

using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<NormalMapTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

NormalMapTest::NormalMapTest()
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createEffects();
    updateProjection();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/plane/brickwall.gltf";


    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(
        scenePath,
        NormalMapEffect::VERTEX_SHADER_ID,
        NormalMapEffect::FRAGMENT_SHADER_ID);

//    for (const auto& material : scene->getMaterials()) {
//        material->setSpecularFactor({1.0f, 1.0f, 1.0f});
//        material->setShininess(100.0f);
//    }

    camera.setPosition({ 0.0f, 2.0f, 10.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::createEffects()
{
    RFX_CHECK_STATE(scene->getLightCount() > 0, "");
    auto light = dynamic_pointer_cast<PointLight>(scene->getLight(0));
    RFX_CHECK_STATE(light != nullptr, "");

    effect = make_unique<NormalMapEffect>(graphicsDevice, scene);
    effectImpl = dynamic_cast<NormalMapEffect*>(effect.get());
    effectImpl->setLight(0, light);

    TestApplication::createEffects();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::createCommandBuffers()
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

void NormalMapTest::drawScene(const shared_ptr<CommandBuffer>& commandBuffer)
{
    const vector<VkDescriptorSet>& meshDescSets = effect->getMeshDescriptorSets();
    const vector<VkDescriptorSet>& materialDescSets = effect->getMaterialDescriptorSets();


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
                materialDescSets[subMesh.materialIndex]);

            commandBuffer->drawIndexed(subMesh.indexCount, subMesh.firstIndex);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::updateProjection()
{
    effectImpl->setProjectionMatrix(calcDefaultProjection());
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::updateSceneData(float deltaTime)
{
    effectImpl->setViewMatrix(camera.getViewMatrix());
    effectImpl->setCameraPosition(camera.getPosition());
    effectImpl->updateSceneDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::updateDevTools()
{
    TestApplication::updateDevTools();

    if (devTools->checkBox("Normal Map", &useNormalMap)) {
        effectImpl->enableNormalMap(useNormalMap);
    }

    const auto& light = static_pointer_cast<PointLight>(scene->getLight(0));
    vec3 lightPos = light->getPosition();
    if (devTools->sliderFloat3("light#0 position", &lightPos.x, -10.0f, 10.0f)) {
        light->setPosition(lightPos);
        effectImpl->setLight(0, static_pointer_cast<PointLight>(light));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
