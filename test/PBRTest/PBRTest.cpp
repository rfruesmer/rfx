#include "rfx/pch.h"
#include "PBRTest.h"
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
        auto theApp = make_shared<PBRTest>();
        theApp->run();
    }
    catch (const exception& ex) {
        RFX_LOG_ERROR << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::initGraphics()
{
    TestApplication::initGraphics();

    loadScene();
    createShadersFor(scene, PBRShader::ID);

    initGraphicsResources();
    buildRenderGraph();
    createCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::loadScene()
{
    const path scenePath = getAssetsDirectory() / "models/teapot/teapot.gltf";

    SceneLoader sceneLoader(graphicsDevice);
    scene = sceneLoader.load(scenePath);

    camera->setPosition({ 0.0f, 2.0f, 10.0f });

    RFX_CHECK_STATE(scene->getLightCount() > 0, "");
    pointLight = dynamic_pointer_cast<PointLight>(scene->getLight(0));
    RFX_CHECK_STATE(pointLight != nullptr, "");
    pointLight->setPosition({ 5.0f, 5.0f, 0.0f });
    pointLight->setColor({ 1.0f, 1.0f, 1.0f });
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::initShaderFactory(MaterialShaderFactory& shaderFactory)
{
    shaderFactory.addAllocator(PBRShader::ID,
        [this] { return make_shared<PBRShader>(graphicsDevice); });
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::updateShaderData()
{
    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == PBRShader::ID, "");

    shader = static_pointer_cast<PBRShader>(materialShaderMap.begin()->first);
    shader->setLight(0, pointLight, camera->getViewMatrix());
    shader->updateDataBuffer();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::createMeshResources()
{
    TestApplication::createMeshResources();

    createMeshDataBuffers(scene);
    createMeshDescriptorSets(scene);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::buildRenderGraph()
{
    renderGraph = make_shared<RenderGraph>(graphicsDevice, sceneDescriptorSet_);
    renderGraph->add(scene, materialShaderMap);
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::cleanup()
{
    shader.reset();
    scene.reset();

    TestApplication::cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------

void PBRTest::updateDevTools()
{
    TestApplication::updateDevTools();


    RFX_CHECK_STATE(materialShaderMap.size() == 1, "");
    RFX_CHECK_STATE(materialShaderMap.begin()->first->getId() == PBRShader::ID, "");
    vector<MaterialPtr> materials = materialShaderMap.begin()->second;
    RFX_CHECK_STATE(materials.size() == 1, "");

    for (const auto& material : materials)
    {
        bool materialNeedsUpdate = false;

        float value = material->getMetallicFactor();
        if (devTools->sliderFloat("metallic", &value, 0.0f, 1.0f)) {
            material->setMetallicFactor(value);
            materialNeedsUpdate = true;
        }

        value = material->getRoughnessFactor();
        if (devTools->sliderFloat("roughness", &value, 0.0f, 1.0f)) {
            material->setRoughnessFactor(value);
            materialNeedsUpdate = true;
        }

        vec3 color = material->getBaseColorFactor();
        if (devTools->colorEdit3("albedo", &color.x)) {
            material->setBaseColorFactor(vec4(color, 1.0f));
            materialNeedsUpdate = true;
        }

        value = material->getOcclusionStrength();
        if (devTools->sliderFloat("ambient", &value, 0.0f, 1.0f)) {
            material->setOcclusionStrength(value);
            materialNeedsUpdate = true;
        }

        if (materialNeedsUpdate)
        {
            PBRShader::MaterialData materialData {
                .baseColor = material->getBaseColorFactor(),
                .metallic = material->getMetallicFactor(),
                .roughness = material->getRoughnessFactor(),
                .ao = material->getOcclusionStrength()
            };

            material->getUniformBuffer()->load(sizeof(PBRShader::MaterialData), &materialData);
        }

        bool lightNeedsUpdate = false;
        const auto& light = static_pointer_cast<PointLight>(scene->getLight(0));
        color = light->getColor();
        if (devTools->colorEdit3("light#0 color", &color.x)) {
            light->setColor(color);
            lightNeedsUpdate = true;
        }

        vec3 lightPos = light->getPosition();
        if (devTools->sliderFloat3("light#0 position", &lightPos.x, -100.0f, 100.0f)) {
            light->setPosition(lightPos);
            lightNeedsUpdate = true;
        }

        if (lightNeedsUpdate)
        {
            shader->setLight(0, light, camera->getViewMatrix());
            updateShaderData();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
