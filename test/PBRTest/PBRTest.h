#pragma once

#include "TestApplication.h"
#include "PBREffect.h"


namespace rfx {

class PBRTest : public TestApplication
{
protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData(float deltaTime) override;
    void updateDevTools() override;

private:
    void loadScene();

protected:
    void createEffects() override;

private:
    void createCommandBuffers() override;
    void drawGeometryNode(
        uint32_t index,
        const std::shared_ptr<CommandBuffer>& commandBuffer);


    std::shared_ptr<PointLight> pointLight;
    PBREffect* effectImpl = nullptr;
};

} // namespace rfx


