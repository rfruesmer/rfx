#include "rfx/pch.h"
#include "NormalMapTest.h"
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
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, NormalMapShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/plane/brickwall.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera->setPosition({ 0.0f, 2.0f, 10.0f });

    RFX_CHECK_STATE(scene->getLightCount() > 0, "");
    light = dynamic_pointer_cast<PointLight>(scene->getLight(0));
    RFX_CHECK_STATE(light != nullptr, "");
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(NormalMapShader::ID,
        [this] { return make_shared<NormalMapShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == NormalMapShader::ID, "");

    shader = static_pointer_cast<NormalMapShader>(materialShaderMap.begin()->first);
    shader->setLight(0, light);
    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::cleanup()
{
    shader.reset();
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void NormalMapTest::updateDevTools()
{
    TestApplication::updateDevTools();

    if (devTools->checkBox("Normal Map", &useNormalMap)) {
        shader->enableNormalMap(useNormalMap);
    }

    const auto& light = static_pointer_cast<PointLight>(scene->getLight(0));
    vec3 lightPos = light->getPosition();
    if (devTools->sliderFloat3("light#0 position", &lightPos.x, -10.0f, 10.0f)) {
        light->setPosition(lightPos);
        shader->setLight(0, static_pointer_cast<PointLight>(light));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
