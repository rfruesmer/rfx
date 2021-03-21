#pragma once

#include "TestApplication.h"
#include "MultiLightEffect.h"


namespace rfx {

class MultiLightTest : public TestApplication
{
public:
    MultiLightTest();

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

    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    MultiLightEffect* effectImpl = nullptr;
};

} // namespace rfx


