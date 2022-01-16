#pragma once

#include "TestApplication.h"
#include "SampleViewerShader.h"


namespace rfx::test {

class SampleViewerTest : public TestApplication
{
public:
    SampleViewerTest();

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
    void buildRenderGraph() override;
    void onModelSelected();
    void destroyScene();

    ModelPtr scene;
    std::shared_ptr<SampleViewerShader> shader;
    int selectedModelIndex = 0;
    bool selectedModelChanged = false;
};

} // namespace rfx