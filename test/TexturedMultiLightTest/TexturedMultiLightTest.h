#pragma once

#include "TestApplication.h"
#include "TexturedMultiLightEffect.h"


namespace rfx {

class TexturedMultiLightTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = TexturedMultiLightEffect::VERTEX_FORMAT;

public:
    TexturedMultiLightTest();

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
    TexturedMultiLightEffect* effectImpl = nullptr;
};

} // namespace rfx


