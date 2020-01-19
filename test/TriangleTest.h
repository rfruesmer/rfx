#pragma once

#include "test/TestApplication.h"
#include "rfx/graphics/VertexColorEffect.h"

namespace rfx
{

class TriangleTest : public TestApplication
{
public:
    explicit TriangleTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initEffects() override;
    void initScene() override;
    void initCommandBuffers() override;

private:
    void createTriangleMesh();

    std::shared_ptr<Mesh> triangleMesh;
    std::shared_ptr<VertexColorEffect> vertexColorEffect;
};

}
