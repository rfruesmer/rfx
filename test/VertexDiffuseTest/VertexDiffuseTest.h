#pragma once

#include "TestApplication.h"
#include "VertexDiffuseEffect.h"


namespace rfx {

class VertexDiffuseTest : public TestApplication
{
public:
    VertexDiffuseTest();

protected:
    void initGraphics() override;

    void createUniformBuffers() override;
    void createDescriptorPool() override;
    void createDescriptorSetLayouts() override;
    void createDescriptorSets() override;
    void createPipelineLayout() override;
    void createPipeline() override;

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
    std::unique_ptr<VertexDiffuseEffect> effect;
};

} // namespace rfx


