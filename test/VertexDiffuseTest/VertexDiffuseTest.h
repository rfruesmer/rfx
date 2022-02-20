#pragma once

#include "TestApplication.h"
#include "VertexDiffuseShader.h"
#include "rfx/scene/Scene.h"


namespace rfx {

class VertexDiffuseTest : public TestApplication
{
public:
    VertexDiffuseTest();

protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void cleanup() override;

private:
    void loadScene();
    void buildRenderGraph();

    ScenePtr scene;
    PointLight light { "pointLight" };
};

} // namespace rfx


