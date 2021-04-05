#include "rfx/pch.h"
#include "TexturedMultiLightTest.h"
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
        auto theApp = make_shared<TexturedMultiLightTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

TexturedMultiLightTest::TexturedMultiLightTest()
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::initGraphics()
{
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, TexturedMultiLightShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/plane/plane.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera->setPosition({ 0.0f, 2.0f, 10.0f });

    pointLight = make_shared<PointLight>("point");
    pointLight->setPosition({ 5.0f, .5f, 5.0f });
    pointLight->setColor({ 0.0f, 1.0f, 0.0f });
    pointLight->setRange(6.0f);

    spotLight = make_shared<SpotLight>("spot");
    spotLight->setPosition({ 0.0f, 10.0f, 0.0f });
    spotLight->setColor({ 0.0f, 0.0f, 1.0f });
    spotLight->setDirection({ 0.0f, -1.0f, 0.0f });
    spotLight->setInnerConeAngle(20.0f);
    spotLight->setOuterConeAngle(30.0f);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(TexturedMultiLightShader::ID,
        [this] { return make_shared<TexturedMultiLightShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == TexturedMultiLightShader::ID, "");

    shader = static_pointer_cast<TexturedMultiLightShader>(materialShaderMap.begin()->first);

    shader->setCamera(camera);
    shader->setLight(0, pointLight);
    shader->setLight(1, spotLight);

    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::cleanup()
{
    shader.reset();
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedMultiLightTest::updateSceneData()
{
    TestApplication::updateSceneData();

    if (shader) {
        shader->onSceneDataUpdated();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
