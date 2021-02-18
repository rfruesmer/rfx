#pragma once

#include "rfx/application/Application.h"

namespace rfx::test {

class ColoredQuad : public Application
{
protected:
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    virtual void buildScene();
    virtual void createVertexBuffer();
    virtual void createIndexBuffer();
    virtual void createUniformBuffers();
    virtual void createDescriptorPool();
    virtual void createDescriptorSetLayout();
    virtual void createDescriptorSets();

    void cleanupSwapChain() override;
    void recreateSwapChain() override;

    void update(int bufferIndex) override;

// scene
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<std::shared_ptr<Buffer>> uniformBuffers;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

private:
    void initGraphics() override;
    void cleanup() override;

    void createRenderPass();
    void createGraphicsPipeline();
    void createCommandBuffers();

    void updateUniformBuffer(uint32_t index);
};

} // namespace rfx::test