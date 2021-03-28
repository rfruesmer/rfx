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
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/cubes/cubes.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera.setPosition({0.0f, 0.0f, 20.0f});
    light.setPosition({5.0f, 5.0f, 2.0f});
    light.setColor({1.0f, 1.0f, 1.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createShaders()
{
    MaterialShaderFactory shaderFactory(
        graphicsDevice,
        descriptorPool,
        getShadersDirectory(),
        VertexDiffuseShader::ID);

    shaderFactory.addAllocator(VertexDiffuseShader::ID,
        [this] { return make_shared<VertexDiffuseShader>(graphicsDevice); });

    for (const auto& material : scene->getMaterials())
    {
        const MaterialShaderPtr shader = shaderFactory.createShaderFor(material);
        initMaterialUniformBuffer(material, shader);
        initMaterialDescriptorSetLayout(material, shader);

        materialShaderMap[shader].push_back(material);
    }

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
        VkPipeline pipeline = TestApplication::createDefaultPipelineFor(shader, pipelineLayout);

        shader->setPipeline(pipelineLayout, pipeline);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createCommandBuffers()
{
    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const vector<VkFramebuffer>& swapChainFrameBuffers = swapChain->getFramebuffers();
    VkCommandPool graphicsCommandPool = graphicsDevice->getGraphicsCommandPool();


    commandBuffers = graphicsDevice->createCommandBuffers(graphicsCommandPool, swapChainFrameBuffers.size());

    for (size_t i = 0; i < commandBuffers.size(); ++i)
    {
        const auto& commandBuffer = commandBuffers[i];

        renderGraph->record(
            commandBuffer,
            sceneDescriptorSet_,
            renderPass,
            swapChainFrameBuffers[i]);
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

