#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class TriangleTest : public TestApplication
{
public:
    explicit TriangleTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initScene() override;
    void initPipeline() override;
    void initDescriptorSet() override;
    void initCommandBuffers() override;

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    VkPipelineShaderStageCreateInfo shaderStages[2];
};

}
