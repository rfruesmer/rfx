#pragma once

#include "TestApplication.h"
#include "PBRShader.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class PBRTest : public TestApplication
{
protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void updateDevTools() override;
    void cleanup() override;

private:
    void loadScene();
    void buildRenderGraph() override;

    ScenePtr scene;
    PointLightPtr pointLight;
    std::shared_ptr<PBRShader> shader;
};

} // namespace rfx


