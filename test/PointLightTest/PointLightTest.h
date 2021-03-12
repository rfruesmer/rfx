#pragma once

#include "TestApplication.h"
#include "PointLightEffect.h"


namespace rfx {

class PointLightTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = PointLightEffect::VERTEX_FORMAT;

public:
    PointLightTest();

protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    PointLightEffect* effectImpl = nullptr;
};

} // namespace rfx


