#pragma once

#include "test/TestApplication.h"

namespace rfx
{

class TexturedQuadTest : public TestApplication
{
public:
    explicit TexturedQuadTest(handle_t instanceHandle);

    void initialize() override;

protected:
    void initScene() override;
    void initDescriptorSetLayout() override;
    void initDescriptorPool();
    void initPipeline() override;
    void initDescriptorSet() override;
    void initCommandBuffers() override;

private:
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    std::unique_ptr<Texture2D> texture;
};

}