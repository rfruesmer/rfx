#include "rfx/pch.h"
#include "PBRTest.h"
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
        auto theApp = make_shared<PBRTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createEffects();
    updateProjection();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/teapot/teapot.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(
        scenePath,
        PBREffect::VERTEX_SHADER_ID,
        PBREffect::FRAGMENT_SHADER_ID);

//    for (const auto& material : scene->getMaterials()) {
//        material->setSpecularFactor({1.0f, 0.0f, 0.0f});
//        material->setShininess(128.0f);
//    }

    camera.setPosition({ 0.0f, 2.0f, 10.0f });

    RFX_CHECK_STATE(scene->getLightCount() > 0, "");
    pointLight = dynamic_pointer_cast<PointLight>(scene->getLight(0));
    RFX_CHECK_STATE(pointLight != nullptr, "");

    pointLight->setPosition({5.0f, 5.0f, 0.0f });
    pointLight->setColor({1.0f, 1.0f, 1.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createEffects()
{
    const path shadersDirectory = getAssetsDirectory() / "shaders";

    // TODO: support for multiple/different materials/effects/shaders per scene
    const shared_ptr<Material>& material = scene->getMaterial(0);


    effect = make_unique<PBREffect>(graphicsDevice, scene);
    effect->loadShaders(material, shadersDirectory);
    effect->setLight(0, pointLight);
    effect->setAlbedo({1.0f, 1.0f, 1.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createUniformBuffers()
{
    effect->createUniformBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createDescriptorPools()
{
    effect->createDescriptorPools();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createDescriptorSetLayouts()
{
    effect->createDescriptorSetLayouts();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createDescriptorSets()
{
    effect->createDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createPipelineLayouts()
{
    TestApplication::createDefaultPipelineLayout(*effect);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createPipelines()
{
    TestApplication::createDefaultPipeline(*effect);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createCommandBuffers()
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

        for (uint32_t j = 0; j < scene->getGeometryNodeCount(); ++j) {
            drawGeometryNode(j, commandBuffer);
        }

        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::drawGeometryNode(
    uint32_t index,
    const shared_ptr<CommandBuffer>& commandBuffer)
{
    const shared_ptr<ModelNode>& geometryNode = scene->getGeometryNode(index);
    const vector<VkDescriptorSet>& meshDescSets = effect->getMeshDescriptorSets();
    const vector<VkDescriptorSet>& materialDescSets = effect->getMaterialDescriptorSets();

    commandBuffer->bindDescriptorSet(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        1,
        meshDescSets[index]);

    for (const auto& mesh : geometryNode->getMeshes()) {
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

void PBRTest::updateProjection()
{
    effect->setProjectionMatrix(calcDefaultProjection());
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::updateSceneData(float deltaTime)
{
    effect->setViewMatrix(camera.getViewMatrix());
    effect->updateSceneDataBuffer();
    effect->updateMaterialDataBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::updateDevTools()
{
    TestApplication::updateDevTools();

    float value = effect->getMetallicFactor();
    if (devTools->sliderFloat("metallic", &value, 0.0f, 1.0f)) {
        effect->setMetallicFactor(value);
    }

    value = effect->getRoughnessFactor();
    if (devTools->sliderFloat("roughness", &value, 0.0f, 1.0f)) {
        effect->setRoughnessFactor(value);
    }

    vec3 color = effect->getAlbedo();
    if (devTools->colorEdit3("albedo", &color.x)) {
        effect->setAlbedo(color);
    }

    value = effect->getAmbientOcclusion();
    if (devTools->sliderFloat("ambient", &value, 0.0f, 1.0f)) {
        effect->setAmbientOcclusion(value);
    }

    const auto& light = static_pointer_cast<PointLight>(scene->getLight(0));
    color = light->getColor();
    if (devTools->colorEdit3("light#0 color", &color.x)) {
        light->setColor(color);
        effect->setLight(0, light);
    }

    vec3 lightPos = light->getPosition();
    if (devTools->sliderFloat3("light#0 position", &lightPos.x, -100.0f, 100.0f)) {
        light->setPosition(lightPos);
        effect->setLight(0, static_pointer_cast<PointLight>(light));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::cleanup()
{
    effect->cleanupSwapChain();
    effect.reset();

    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::cleanupSwapChain()
{
    if (effect) {
        effect->cleanupSwapChain();
    }

    TestApplication::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------
