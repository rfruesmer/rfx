#pragma once

#include "TestApplication.h"
#include "NormalMapEffect.h"


namespace rfx {

class NormalMapTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = NormalMapEffect::VERTEX_FORMAT;

public:
    NormalMapTest();

protected:
    void initGraphics() override;
    void updateProjection() override;
    void updateSceneData(float deltaTime) override;
    void updateDevTools() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    NormalMapEffect* effectImpl = nullptr;
    bool useNormalMap = true;

//    float lightRotSpeed = glm::pi<float>() / 4000.0f;
//    float lightRotAngle = 100.0f;
};

} // namespace rfx


