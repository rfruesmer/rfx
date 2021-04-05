#pragma once

#include "TestApplication.h"
#include "rfx/scene/SkyBox.h"


namespace rfx {

class CubeMapTest : public TestApplication
{
protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override {}
    void createSceneResources() override;
    void buildRenderGraph() override;

    void updateShaderData() override;

    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();

    SkyBoxPtr skyBox;
};

} // namespace rfx
