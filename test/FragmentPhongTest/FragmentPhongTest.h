#pragma once

#include "TestApplication.h"
#include "FragmentPhongEffect.h"


namespace rfx {

class FragmentPhongTest : public TestApplication
{
    static const inline VertexFormat VERTEX_FORMAT = FragmentPhongEffect::VERTEX_FORMAT;

public:
    FragmentPhongTest();

protected:
    void initGraphics() override;

private:
    void loadScene();
    void createCommandBuffers() override;
    void drawScene(const std::shared_ptr<CommandBuffer>& commandBuffer);

    PointLight light;
    FragmentPhongEffect* effectImpl = nullptr;
};

} // namespace rfx


