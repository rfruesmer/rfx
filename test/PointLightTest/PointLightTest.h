#pragma once

#include "TestApplication.h"
#include "PointLightShader.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class PointLightTest : public TestApplication
{
public:
    PointLightTest();

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
    PointLight light { "pointLight" };
};

} // namespace rfx


