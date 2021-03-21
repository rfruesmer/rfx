#pragma once

#include "TestApplication.h"
#include "SkyBoxEffect.h"


namespace rfx {

class CubeMapTest : public TestApplication
{
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

    std::shared_ptr<Model> skyBoxModel;
    std::unique_ptr<SkyBoxEffect> skyBoxEffect;
};

} // namespace rfx
