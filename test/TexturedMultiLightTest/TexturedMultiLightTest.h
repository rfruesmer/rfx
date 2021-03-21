#pragma once

#include "TestApplication.h"
#include "TexturedMultiLightEffect.h"


namespace rfx {

class TexturedMultiLightTest : public TestApplication
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
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<Model> scene;
    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    std::unique_ptr<TexturedMultiLightEffect> effect;
};

} // namespace rfx


