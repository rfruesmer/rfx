#pragma once

#include "TestApplication.h"


namespace rfx {

class CubeMapTest : public TestApplication
{
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

    std::shared_ptr<Model> skyBox;

//    PointLight light;
//    VertexDiffuseEffect* effectImpl = nullptr;
};

} // namespace rfx
