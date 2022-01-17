#pragma once

#include "TestApplication.h"
#include "SampleViewerShader.h"
#include "rfx/scene/DirectionalLight.h"


namespace rfx::test {

class SampleViewerTest : public TestApplication
{
public:
    SampleViewerTest() = default;

protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void updateDevTools() override;
    void update(float deltaTime) override;
    void cleanup() override;

private:
    void loadScene();
    void createLights();
    void buildRenderGraph() override;
    void onModelSelected();
    void destroyScene();

    ModelPtr scene;
    std::shared_ptr<SampleViewerShader> shader;

    int selectedModelIndex = 0;
    bool selectedModelChanged = false;
    bool imageBasedLighting = false;
    DirectionalLightPtr directionalLight;
};

} // namespace rfx