#include "rfx/pch.h"
#include "MultiLightTest.h"
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
        auto theApp = make_shared<MultiLightTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

MultiLightTest::MultiLightTest()
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::initGraphics()
{
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, MultiLightShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/plane/plane.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera->setPosition({ 0.0f, 2.0f, 10.0f });

    pointLight = make_shared<PointLight>("point");
    pointLight->setPosition({ 5.0f, .5f, 5.0f });
    pointLight->setColor({ 1.0f, 1.0f, 1.0f });
    pointLight->setRange(6.0f);

    spotLight = make_shared<SpotLight>("spot");
    spotLight->setPosition({ 0.0f, 10.0f, 0.0f });
    spotLight->setColor({ 1.0f, 1.0f, 1.0f });
    spotLight->setDirection({ 0.0f, -1.0f, 0.0f });
    spotLight->setInnerConeAngle(20.0f);
    spotLight->setOuterConeAngle(30.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(MultiLightShader::ID,
        [this] { return make_shared<MultiLightShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == MultiLightShader::ID, "");

    shader = static_pointer_cast<MultiLightShader>(materialShaderMap.begin()->first);

    shader->setCamera(camera);
    shader->setLight(0, pointLight);
    shader->setLight(1, spotLight);

    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::cleanup()
{
    shader.reset();
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void MultiLightTest::updateSceneData()
{
    TestApplication::updateSceneData();

    if (shader) {
        shader->onSceneDataUpdated();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
