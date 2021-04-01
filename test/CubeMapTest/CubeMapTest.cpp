#include "rfx/pch.h"
#include "CubeMapTest.h"
#include "SkyBoxShader.h"
#include "ReflectionShader.h"
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
        auto theApp = make_shared<CubeMapTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::initGraphics()
{
    Application::initGraphics();

    loadScene();
    createDescriptorPool();
    createShadersFor(skyBoxModel, SkyBoxShader::ID);
    updateProjection();

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::loadScene()
{
    const path skyBoxPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/models/cube.gltf";

    ModelLoader modelLoader(graphicsDevice);
    skyBoxModel = modelLoader.load(skyBoxPath);

    camera->setPosition({0.0f, 1.0f, 20.0f});
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(SkyBoxShader::ID,
        [this] { return make_shared<SkyBoxShader>(graphicsDevice); });

    shaderFactory.addAllocator(ReflectionShader::ID,
        [this] { return make_shared<ReflectionShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateShaderData()
{
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(skyBoxModel);
    createMeshDescriptorSets(skyBoxModel);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice);
    renderGraph->add(skyBoxModel, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::cleanup()
{
    skyBoxShader.reset();
    skyBoxModel.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateDevTools()
{
    TestApplication::updateDevTools();
}

// ---------------------------------------------------------------------------------------------------------------------
