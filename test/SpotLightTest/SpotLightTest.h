#pragma once

#include "TestApplication.h"
#include "SpotLightShader.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class SpotLightTest : public TestApplication
{
public:
    SpotLightTest();

protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void cleanup() override;

private:
    void loadScene();
    void buildRenderGraph() override;

    ScenePtr scene;
};

} // namespace rfx


