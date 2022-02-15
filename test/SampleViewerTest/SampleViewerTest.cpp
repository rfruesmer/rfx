#include "rfx/pch.h"
#include "SampleViewerTest.h"
#include "rfx/scene/ModelLoader.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace rfx::test;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

static const char* models[] = {
    "Box",
    "BoxTextured"
};

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<SampleViewerTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::initGraphics()
{
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, SampleViewerShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::loadScene()
{
    path scenePath = getAssetsDirectory()
        / "models"
        / "glTF-Sample-Models"
        / "2.0"
        / models[selectedModelIndex]
        / "glTF"
        / models[selectedModelIndex];

    scenePath.replace_extension("gltf");

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);
    if (scene->getLightCount() > 0) {
        RFX_THROW_NOT_IMPLEMENTED();
    }

    createLights();

    camera->setPosition({ 0.0f, 0.0f, 2.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::createLights()
{
    directionalLight = make_shared<DirectionalLight>("directional-light#0");
    directionalLight->setColor({1.0F, 1.0F, 1.0F});
    directionalLight->setDirection({ -1.0F, -1.0F, -1.0F });

    scene->addLight(directionalLight);
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::buildRenderGraph()
{
    skyBoxNode = make_shared<SkyBoxNode>(skyBox);

    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(skyBoxNode);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(SampleViewerShader::ID,
        [this] { return make_shared<SampleViewerShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::createSceneResources()
{
    TestApplication::createSceneResources();

    createSkyBox(); // needs to be executed after creation of render pass
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::createSkyBox()
{
    const path skyBoxModelPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/models/cube.gltf";
    const path skyBoxCubeMapPath = getAssetsDirectory() / "environments/footprint_court.hdr";
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

void SampleViewerTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == SampleViewerShader::ID, "");

    shader = static_pointer_cast<SampleViewerShader>(materialShaderMap.begin()->first);
    shader->setLight(0, directionalLight);
    shader->updateDataBuffer();

    if (skyBox != nullptr) {
        skyBox->updateUniformBuffer(camera);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::updateDevTools()
{
    // Models
    if (devTools->combo("Models", IM_ARRAYSIZE(models), models, &selectedModelIndex)) {
        needsReload = true;
    }

    // Lighting
    static bool lightingExpanded = true;
    lightingExpanded = devTools->collapsingHeader("Lighting", lightingExpanded);
    if (lightingExpanded) {
        devTools->checkBox("Image Based", &imageBasedLighting);

        bool lightNeedsUpdate = false;

        bool enabled = directionalLight->isEnabled();
        if (devTools->checkBox("directional light enabled", &enabled)) {
            directionalLight->setEnabled(enabled);
            lightNeedsUpdate = true;
        }

        vec3 color = directionalLight->getColor();
        if (devTools->colorEdit3("directional light color", &color.x)) {
            directionalLight->setColor(color);
            lightNeedsUpdate = true;
        }

        vec3 lightDir = directionalLight->getDirection();
        if (devTools->sliderFloat3("directional light direction", &lightDir.x, -1.0f, 1.0f)) {
            directionalLight->setDirection(lightDir);
            lightNeedsUpdate = true;
        }

        if (lightNeedsUpdate) {
            updateShaderData();
        }
    }

    // Background
    static bool backgroundExpanded = true;
    backgroundExpanded = devTools->collapsingHeader("Background", backgroundExpanded);
    if (backgroundExpanded)
    {
        if (devTools->checkBox("Environment Map", &useEnvironmentMap)) {
            skyBoxNode->setEnabled(useEnvironmentMap);
            freeCommandBuffers();
            createCommandBuffers();
        }

        if (devTools->sliderFloat("Blur", &environmentBlurFactor, 0.0f, 1.0f)) {
            skyBox->setBlur(environmentBlurFactor);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::update(float deltaTime)
{
    TestApplication::update(deltaTime);

    if (needsReload) {
        reload();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::reload()
{
    needsReload = false;

    destroyScene();
    destroyShaderMap();
    destroyRenderGraph();
    destroyRenderPass();
    destroyMeshResources();
    destroySceneResources();

    loadScene();
    createShadersFor(scene, SampleViewerShader::ID);

    createRenderPass();
    createSceneResources();
    createMeshResources();
    createPipelines();

    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::destroyScene()
{
    shader.reset();
    scene.reset();
    skyBox.reset();

    camera->clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::cleanup()
{
    destroyScene();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::cleanupSwapChain()
{
    if (skyBox != nullptr) {
        skyBox->cleanupSwapChain();
    }

    skyBoxNode.reset();

    TestApplication::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------

