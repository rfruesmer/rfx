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
    createDescriptorPool();
    createShaders();
    updateProjection();

    initGraphicsResources();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/cubes/cubes.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera.setPosition({0.0f, 1.0f, 2.0f});
    light.setPosition({5.0f, 5.0f, 2.0f});
    light.setColor({1.0f, 1.0f, 1.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createShaders()
{
    MaterialShaderFactory shaderFactory(graphicsDevice, getShadersDirectory(), VertexDiffuseShader::ID);

    shaderFactory.addAllocator(VertexDiffuseShader::ID,
        [this] { return make_shared<VertexDiffuseShader>(graphicsDevice); });

    for (const auto& material : scene->getMaterials()) {
        const MaterialShaderPtr shader = shaderFactory.createShaderFor(material);


        const VertexDiffuseShader::MaterialData materialData {
            .baseColor = material->getBaseColorFactor(),
            .specular = material->getSpecularFactor(),
            .shininess = material->getShininess()
        };

        material->setUniformBuffer(
            createAndBindUniformBuffer(sizeof(materialData), &materialData));

        material->createDescriptorSet(descriptorPool, shader->getMaterialDescriptorSetLayout());
        materialShaderMap[shader].push_back(material);
    }

    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    vertexDiffuseShader = static_pointer_cast<VertexDiffuseShader>(materialShaderMap.begin()->first);

    setLight(light);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createMeshResources()
{
    createMeshDataBuffers(scene);
    createMeshDescriptorSetLayout();
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createPipelines()
{
    for (const auto& [shader, materials] : materialShaderMap)
    {
        VkDescriptorSetLayout materialDescriptorSetLayout = shader->getMaterialDescriptorSetLayout();

        vector<VkDescriptorSetLayout> descriptorSetLayouts {
            sceneDescriptorSetLayout_,
            meshDescriptorSetLayout_,
            materialDescriptorSetLayout
        };

        VkPipelineLayout pipelineLayout = TestApplication::createDefaultPipelineLayout(descriptorSetLayouts);
        VkPipeline pipeline = TestApplication::createDefaultPipelineFor(*vertexDiffuseShader, pipelineLayout);

        shader->setPipeline(pipelineLayout, pipeline);
    }
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
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, wireframe ? wireframePipeline : materialShaderMap.begin()->first->getPipeline());
        commandBuffer->bindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, materialShaderMap.begin()->first->getPipelineLayout(), 0, sceneDescriptorSet_);
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


    for (const auto& mesh : geometryNode->getMeshes()) {

        commandBuffer->bindDescriptorSet(
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            materialShaderMap.begin()->first->getPipelineLayout(),
            1,
            mesh->getDescriptorSet());

        for (const auto& subMesh : mesh->getSubMeshes()) {
            if (subMesh.indexCount == 0) {
                continue;
            }

            commandBuffer->bindDescriptorSet(
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                materialShaderMap.begin()->first->getPipelineLayout(),
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
    VkDevice device = graphicsDevice->getLogicalDevice();

    for (const auto& [shader, materials] : materialShaderMap) {
        shader->destroyMaterialDescriptorSetLayout();

        vkDestroyPipeline(device, shader->getPipeline(), nullptr);
        vkDestroyPipelineLayout(device, shader->getPipelineLayout(), nullptr);
        shader->setPipeline(VK_NULL_HANDLE, VK_NULL_HANDLE);
    }

    vertexDiffuseShader.reset();
    materialShaderMap.clear();
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::cleanupSwapChain()
{
    TestApplication::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------
