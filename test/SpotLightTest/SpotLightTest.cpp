#include "rfx/pch.h"
#include "SpotLightTest.h"
#include "rfx/scene/ModelLoader.h"
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
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, SpotLightShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/plane/plane_with_spotlight.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera->setPosition({0.0f, 1.0f, 2.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(SpotLightShader::ID,
        [this] { return make_shared<SpotLightShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == SpotLightShader::ID, "");

    RFX_CHECK_STATE(scene->getLightCount() > 0, "");
    auto light = dynamic_pointer_cast<SpotLight>(scene->getLight(0));
    RFX_CHECK_STATE(light != nullptr, "");

    const SpotLightShaderPtr shader = static_pointer_cast<SpotLightShader>(materialShaderMap.begin()->first);

    shader->setLight(
        sceneData_.viewMatrix * vec4(light->getPosition(), 1.0f),
        light->getColor(),
        light->getDirection(),
        light->getInnerConeAngle(),
        light->getOuterConeAngle());

    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void SpotLightTest::cleanup()
{
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------
