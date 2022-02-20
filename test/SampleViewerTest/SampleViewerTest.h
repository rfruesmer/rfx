#pragma once

#include "TestApplication.h"
#include "SampleViewerShader.h"
#include "rfx/scene/Scene.h"
#include "rfx/scene/DirectionalLight.h"
#include "rfx/scene/SkyBox.h"
#include "rfx/rendering/SkyBoxNode.h"


namespace rfx::test {

class SampleViewerTest : public TestApplication
{
public:
    SampleViewerTest() = default;

protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createSceneResources() override;
    void createMeshResources() override;
    void updateShaderData() override;
    void updateDevTools() override;
    void update(float deltaTime) override;
    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();
    void createLights();
    void createSkyBox();
    void buildRenderGraph() override;
    void reload();
    void destroyScene();

    ScenePtr scene;

    SkyBoxPtr skyBox;
    SkyBoxNodePtr skyBoxNode;

    int selectedModelIndex = 2;
    bool needsReload = false;
    bool imageBasedLighting = false;
    DirectionalLightPtr directionalLight;

    bool useEnvironmentMap = true;
    float environmentBlurFactor = 0.0f;
};

} // namespace rfx