#pragma once

#include "TestApplication.h"
#include "SpotLightEffect.h"


namespace rfx {

class SpotLightTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = SpotLightEffect::VERTEX_FORMAT;

public:
    SpotLightTest();

protected:
    void initGraphics() override;

    void updateProjection() override;
    void updateSceneData() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    SpotLightEffect* effectImpl = nullptr;
};

} // namespace rfx


