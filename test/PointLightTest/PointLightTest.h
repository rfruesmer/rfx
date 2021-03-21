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
    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

private:
    void loadScene();

protected:
    void createEffects() override;

private:
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    PointLightEffect* effectImpl = nullptr;
};

} // namespace rfx


