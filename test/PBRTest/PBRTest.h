#pragma once

#include "TestApplication.h"
#include "PBREffect.h"


namespace rfx {

class PBRTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = PBREffect::VERTEX_FORMAT;

protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData() override;
    void updateDevTools() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawGeometryNode(
        uint32_t index,
        const std::shared_ptr<CommandBuffer>& commandBuffer);


    std::shared_ptr<PointLight> pointLight;
    std::shared_ptr<SpotLight> spotLight;
    PBREffect* effectImpl = nullptr;
};

} // namespace rfx


