#include "rfx/pch.h"
#include "VertexDiffuseTest.h"
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
        auto theApp = make_shared<VertexDiffuseTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

VertexDiffuseTest::VertexDiffuseTest()
    : light("point")
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createEffects();
    updateProjection();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/cubes/cubes.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(
        scenePath,
        VertexDiffuseShader::VERTEX_SHADER_ID,
        VertexDiffuseShader::FRAGMENT_SHADER_ID);

    camera.setPosition({0.0f, 1.0f, 2.0f});
    light.setPosition({5.0f, 5.0f, 2.0f});
    light.setColor({1.0f, 1.0f, 1.0f});

}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createEffects()
{
    const path shadersDirectory = getAssetsDirectory() / "shaders";

    // TODO: support for multiple/different materials/effects/shaders per scene
    const shared_ptr<Material>& material = scene->getMaterial(0);


    effect = make_unique<VertexDiffuseShader>(graphicsDevice, scene);
    effect->loadShaders(material, shadersDirectory);
    setLight(light);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createUniformBuffers()
{
    createSceneDataBuffer();

    effect->createUniformBuffers();

    const VkDeviceSize bufferSize = sizeof(VertexDiffuseShader::MaterialData);

    for (const auto& material : scene->getMaterials())
    {
        const VertexDiffuseShader::MaterialData materialData {
            .baseColor = material->getBaseColorFactor(),
            .specular = material->getSpecularFactor(),
            .shininess = material->getShininess()
        };

        material->setUniformBuffer(
            createAndBindUniformBuffer(bufferSize, &materialData));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createDescriptorSetLayouts()
{
    createSceneDescriptorSetLayout();

    effect->createDescriptorSetLayouts();

    for (const auto& material : scene->getMaterials()) {
        material->createDescriptorSetLayout();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createDescriptorSets()
{
    createSceneDescriptorSet();

    effect->createDescriptorSets(descriptorPool);

    for (const auto& material : scene->getMaterials()) {
        material->createDescriptorSet(descriptorPool);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createPipelineLayouts()
{
    vector<VkDescriptorSetLayout> descriptorSetLayouts {
        sceneDescriptorSetLayout_
    };

    ranges::copy(effect->getDescriptorSetLayouts(), back_inserter(descriptorSetLayouts));
    descriptorSetLayouts.push_back(scene->getMaterial(0)->getDescriptorSetLayout());

    TestApplication::createDefaultPipelineLayout(descriptorSetLayouts);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createPipelines()
{
    TestApplication::createDefaultPipeline(*effect);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createCommandBuffers()
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
        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, sceneDescriptorSet_);
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

void VertexDiffuseTest::drawGeometryNode(
    uint32_t index,
    const shared_ptr<CommandBuffer>& commandBuffer)
{
    const shared_ptr<ModelNode>& geometryNode = scene->getGeometryNode(index);
    const vector<VkDescriptorSet>& meshDescSets = effect->getMeshDescriptorSets();


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
                subMesh.material->getDescriptorSet());

            commandBuffer->drawIndexed(subMesh.indexCount, subMesh.firstIndex);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::updateProjection()
{
    setProjectionMatrix(calcDefaultProjection());
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::updateSceneData(float deltaTime)
{
    setViewMatrix(camera.getViewMatrix());
    updateSceneDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::cleanup()
{
    effect->cleanupSwapChain();
    effect.reset();

    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::cleanupSwapChain()
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
