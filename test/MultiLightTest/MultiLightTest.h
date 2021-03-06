#pragma once

#include "TestApplication.h"
#include "MultiLightEffect.h"


namespace rfx {

class MultiLightTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = MultiLightEffect::VERTEX_FORMAT;

public:
    MultiLightTest();

protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    MultiLightEffect* effectImpl = nullptr;
};

} // namespace rfx


