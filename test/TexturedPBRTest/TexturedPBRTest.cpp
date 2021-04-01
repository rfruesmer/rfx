#include "rfx/pch.h"
#include "TexturedPBRTest.h"
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
        auto theApp = make_shared<TexturedPBRTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createDescriptorPool();
    createShadersFor(scene, TexturedPBRShader::ID);
    updateProjection();

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/sci-fi-corridors/scene.gltf";
//    const path scenePath = getAssetsDirectory() / "models/sci-fi_modular_corridor__door_ver.2_-_low_poly/scene.gltf";
//    const path scenePath = getAssetsDirectory() / "samples/NormalTangentMirrorTest/glTF/NormalTangentMirrorTest.gltf";
//    const path scenePath = getAssetsDirectory() / "models/plane/plane_pbr.gltf";
//    const path scenePath = getAssetsDirectory() / "models/cubes/ice_low.gltf";

    ModelLoader modelLoader(graphicsDevice);
    scene = modelLoader.load(scenePath);

    camera->setPosition({ 0.0f, 2.0f, 10.0f });

    if (scene->getLightCount() > 0) {
        pointLight = dynamic_pointer_cast<PointLight>(scene->getLight(0));
    }

    if (pointLight == nullptr) {
        pointLight = make_shared<PointLight>("point-light#0");
        pointLight->setPosition({-5.3f, 2.7f, -3.3f });
        pointLight->setColor({0.5f, 0.5f, 0.5f});
        scene->addLight(pointLight);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(TexturedPBRShader::ID,
        [this] { return make_shared<TexturedPBRShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == TexturedPBRShader::ID, "");

    shader = static_pointer_cast<TexturedPBRShader>(materialShaderMap.begin()->first);
    shader->setLight(0, pointLight);
    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::cleanup()
{
    shader.reset();
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void TexturedPBRTest::updateDevTools()
{
    TestApplication::updateDevTools();

    const auto& light = static_pointer_cast<PointLight>(scene->getLight(0));
    bool lightNeedsUpdate = false;

    vec3 color = light->getColor();
    if (devTools->colorEdit3("light#0 color", &color.x)) {
        light->setColor(color);
        lightNeedsUpdate = true;
    }

    vec3 lightPos = light->getPosition();
    if (devTools->sliderFloat3("light#0 position", &lightPos.x, -10.0f, 10.0f)) {
        light->setPosition(lightPos);
        lightNeedsUpdate = true;
    }

    if (lightNeedsUpdate) {
        shader->setLight(0, light);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
