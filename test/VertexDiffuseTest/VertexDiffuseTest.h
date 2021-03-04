#pragma once

#include "TestApplication.h"
#include "VertexDiffuseEffect.h"


namespace rfx {

class VertexDiffuseTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = VertexDiffuseEffect::VERTEX_FORMAT;

public:
    VertexDiffuseTest();

protected:
    void initGraphics() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    VertexDiffuseEffect* effectImpl = nullptr;
};

} // namespace rfx


