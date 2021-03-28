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
        initMaterialDescriptorSet(material, shader);

        materialShaderMap[shader].push_back(material);
    }

    updateShaderData();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == VertexDiffuseShader::ID, "");

    const VertexDiffuseShaderPtr shader = static_pointer_cast<VertexDiffuseShader>(materialShaderMap.begin()->first);

    shader->setLightPosition(sceneData_.viewMatrix * vec4(light.getPosition(), 1.0f));
    shader->setLightAmbient({ 0.01f, 0.01f, 0.01f });
    shader->setLightDiffuse({ 0.7f, 0.7f, 0.7f });
    shader->setLightSpecular({ 0.3f, 0.3f, 0.3f });

    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::createMeshResources()
{
    createMeshDataBuffers(scene);
    createMeshDescriptorSetLayout();
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDiffuseTest::cleanup()
{
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

