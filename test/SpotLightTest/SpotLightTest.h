#pragma once

#include "TestApplication.h"
#include "SpotLightEffect.h"


namespace rfx {

class SpotLightTest : public TestApplication
{
public:
    SpotLightTest();

protected:
    void initGraphics() override;

    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

private:
    void loadScene();

protected:
    void createEffects() override;

private:
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    SpotLightEffect* effectImpl = nullptr;
};

} // namespace rfx


