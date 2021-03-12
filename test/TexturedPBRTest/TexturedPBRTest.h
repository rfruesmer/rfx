#pragma once

#include "TestApplication.h"
#include "TexturedPBREffect.h"


namespace rfx {

class TexturedPBRTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = TexturedPBREffect::VERTEX_FORMAT;

protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData(float deltaTime) override;
    void updateDevTools() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawGeometryNode(
        uint32_t index,
        const std::shared_ptr<CommandBuffer>& commandBuffer);


    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    TexturedPBREffect* effectImpl = nullptr;
};

} // namespace rfx


