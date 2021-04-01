#pragma once

#include "TestApplication.h"
#include "SkyBoxShader.h"


namespace rfx {

class CubeMapTest : public TestApplication
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
    void buildRenderGraph();

    ModelPtr skyBoxModel;
    std::shared_ptr<SkyBoxShader> skyBoxShader;
};

} // namespace rfx
