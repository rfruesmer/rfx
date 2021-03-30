#pragma once

#include "TestApplication.h"
#include "NormalMapShader.h"


namespace rfx {

class NormalMapTest : public TestApplication
{
public:
    NormalMapTest();

protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void updateDevTools() override;
    void cleanup() override;

private:
    void loadScene();
    void buildRenderGraph();

    ModelPtr scene;
    PointLightPtr light;
    std::shared_ptr<NormalMapShader> shader;
    bool useNormalMap = true;
};

} // namespace rfx


