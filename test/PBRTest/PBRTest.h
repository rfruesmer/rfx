#pragma once

#include "TestApplication.h"
#include "PBREffect.h"


namespace rfx {

class PBRTest : public TestApplication
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
    void updateDevTools() override;

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
    std::shared_ptr<PointLight> pointLight;
    std::unique_ptr<PBREffect> effect;
};

} // namespace rfx


