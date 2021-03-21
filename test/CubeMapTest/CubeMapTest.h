#pragma once

#include "TestApplication.h"


namespace rfx {

class CubeMapTest : public TestApplication
{
protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData(float deltaTime) override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawGeometryNode(
        uint32_t index,
        const std::shared_ptr<CommandBuffer>& commandBuffer);

//    PointLight light;
//    VertexDiffuseEffect* effectImpl = nullptr;

};

} // namespace rfx
