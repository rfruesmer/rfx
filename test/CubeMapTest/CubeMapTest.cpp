#include "rfx/pch.h"
#include "CubeMapTest.h"
#include "rfx/rendering/SkyBoxNode.h"
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
    TestApplication::initGraphics();

    loadScene();

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::loadScene()
{
    camera->setPosition({ 0.0f, 0.0f, 0.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::updateShaderData()
{
    skyBox->updateUniformBuffer(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::createSceneResources()
{
    TestApplication::createSceneResources();


    const path skyBoxModelPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/models/cube.gltf";
    const path skyBoxCubeMapPath = getAssetsDirectory() / "models/vulkan_asset_pack_gltf/textures/cubemap_yokohama_rgba.ktx";
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

void CubeMapTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(make_shared<SkyBoxNode>(skyBox));
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::cleanup()
{
    skyBox.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void CubeMapTest::cleanupSwapChain()
{
    if (skyBox != nullptr) {
        skyBox->cleanupSwapChain();
    }

    TestApplication::cleanupSwapChain();
}

// ---------------------------------------------------------------------------------------------------------------------
