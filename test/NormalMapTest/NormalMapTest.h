#pragma once

#include "TestApplication.h"
#include "NormalMapEffect.h"


namespace rfx {

class NormalMapTest : public TestApplication
{
public:
    NormalMapTest();

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
    void updateDevTools() override;

    void cleanup() override;
    void cleanupSwapChain() override;

private:
    void loadScene();

protected:
    void createEffects() override;

private:
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<Model> scene;
    std::unique_ptr<NormalMapEffect> effect;
    bool useNormalMap = true;
};

} // namespace rfx


