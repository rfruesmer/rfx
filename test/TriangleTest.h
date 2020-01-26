#pragma once

#include "test/TestApplication.h"
#include "rfx/graphics/effect/VertexColorEffect.h"

namespace rfx
{

class TriangleTest : public TestApplication
{
public:
    explicit TriangleTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initEffects();
    void initScene() override;
    [[nodiscard]] const std::shared_ptr<Camera>& getCamera() const override;
    void initCommandBuffers() override;

private:
    void createTriangleMesh();
    void initCamera();

    std::shared_ptr<Camera> camera;
    std::shared_ptr<Mesh> triangleMesh;
    std::shared_ptr<VertexColorEffect> vertexColorEffect;
};

}
