#pragma once

#include "TestApplication.h"
#include "VertexDiffuseShader.h"


namespace rfx {

class VertexDiffuseTest : public TestApplication
{
public:
    VertexDiffuseTest();

protected:
    void initGraphics() override;
    void createShaders() override;
    void createMeshResources() override;
    void buildRenderGraph();

    void updateShaderData() override;

    void cleanup() override;

private:
    void loadScene();

    ModelPtr scene;
    PointLight light { "point" };
};

} // namespace rfx


