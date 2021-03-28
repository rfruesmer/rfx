#pragma once

#include "TestApplication.h"
#include "SpotLightShader.h"


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
    void buildRenderGraph();

    ModelPtr scene;
};

} // namespace rfx


