#pragma once

#include "test/TestApplication.h"
#include "rfx/graphics/VertexColorEffect.h"

namespace rfx
{

class CubeTest : public TestApplication
{
public:
    CubeTest(handle_t instanceHandle);

    void initialize() override;

protected:
    CubeTest(const std::filesystem::path& configurationPath, handle_t instanceHandle);

    void initScene() override;
    void initCommandBuffers() override;

private:
    void drawNode(const std::unique_ptr<SceneNode>& sceneNode,
        const std::shared_ptr<CommandBuffer>& commandBuffer);

    std::shared_ptr<VertexColorEffect> vertexColorEffect;
};

} // namespace rfx

