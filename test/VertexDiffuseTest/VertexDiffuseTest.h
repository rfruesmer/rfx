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
    void createMeshResources() override;
    void buildRenderGraph();

    void setViewMatrix(const glm::mat4& viewMatrix) override;
    void updateShaderData();

    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();
    void createShaders() override;


    std::shared_ptr<Model> scene;
    PointLight light { "point" };
};

} // namespace rfx


