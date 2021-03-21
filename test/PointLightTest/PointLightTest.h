#pragma once

#include "TestApplication.h"
#include "PointLightEffect.h"


namespace rfx {

class PointLightTest : public TestApplication
{
public:
    PointLightTest();

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
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<Model> scene;
    std::unique_ptr<PointLightEffect> effect;
};

} // namespace rfx


