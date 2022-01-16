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
    "Box"
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

SampleViewerTest::SampleViewerTest() {}

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

    camera->setPosition({ 0.0f, 2.0f, 10.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(SampleViewerShader::ID,
        [this] { return make_shared<SampleViewerShader>(graphicsDevice); });
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
    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::updateDevTools()
{
    if (devTools->combo("Models", IM_ARRAYSIZE(models), models, &selectedModelIndex)) {
        selectedModelChanged = true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::update(float deltaTime)
{
    TestApplication::update(deltaTime);

    if (selectedModelChanged) {
        onModelSelected();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::onModelSelected()
{
    selectedModelChanged = false;

    destroyScene();
    destroyShaderMap();
    destroyRenderGraph();
    destroyRenderPass();
    destroyMeshResources();
    destroySceneResources();

    loadScene();
    createShadersFor(scene, SampleViewerShader::ID);

//    initGraphicsResources();
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

    camera->clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void SampleViewerTest::cleanup()
{
    destroyScene();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

