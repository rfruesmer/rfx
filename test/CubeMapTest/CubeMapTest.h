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
    void createCommandBuffers() override;

    void updateShaderData() override;

    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();
    void buildRenderGraph();

private:
    SkyBoxPtr skyBox;
};

} // namespace rfx
