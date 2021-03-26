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

    void createUniformBuffers() override;
    void createDescriptorPools() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;
    void createPipelineLayouts() override;
    void createPipelines() override;

    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();
    void createEffects() override;
    void createCommandBuffers() override;
    void drawGeometryNode(
        uint32_t index,
        const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<Model> scene;
    PointLight light;
    std::unique_ptr<VertexDiffuseShader> effect;
};

} // namespace rfx


