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

protected:
    void createEffects() override;

private:
    void createCommandBuffers() override;
    void drawGeometryNode(
        uint32_t index,
        const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<Model> skyBox;

//    PointLight light;
//    VertexDiffuseEffect* effectImpl = nullptr;
};

} // namespace rfx
