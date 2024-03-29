#include "rfx/pch.h"
#include "PointLightTest.h"
#include "rfx/scene/SceneLoader.h"
#include "rfx/common/Logger.h"


using namespace rfx;
using namespace glm;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    try {
        auto theApp = make_shared<PointLightTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

PointLightTest::PointLightTest()
{
    devToolsEnabled = true;
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::initGraphics()
{
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, PointLightShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/spheres/spheres.gltf";

    SceneLoader sceneLoader(graphicsDevice);
    scene = sceneLoader.load(scenePath);

    const ModelPtr& model = scene->getModel(0);
    for (const auto& material : model->getMaterials()) {
        material->setSpecularFactor({1.0f, 1.0f, 1.0f});
        material->setShininess(100.0f);
    }

    camera->setPosition({ 0.0f, 1.0f, 10.0f });

    light.setColor({ 1.0f, 1.0f, 1.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(PointLightShader::ID,
        [this] { return make_shared<PointLightShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == PointLightShader::ID, "");

    const PointLightShaderPtr shader = static_pointer_cast<PointLightShader>(materialShaderMap.begin()->first);

    shader->setLightPosition(sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f));
    shader->setLightColor(light.getColor());

    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void PointLightTest::cleanup()
{
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------
