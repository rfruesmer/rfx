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
    void createPipelines() override;
    void buildRenderGraph();

    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();
    void createShaders() override;
    void createCommandBuffers() override;


    std::shared_ptr<Model> scene;
    PointLight light;
    std::unordered_map<MaterialShaderPtr, std::vector<MaterialPtr>> materialShaderMap;
};

} // namespace rfx


