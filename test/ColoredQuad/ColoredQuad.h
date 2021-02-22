#pragma once


#include "rfx/application/Application.h"

namespace rfx::test {

class ColoredQuad : public Application
{
private:
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    void initGraphics() override;
    void buildScene();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createDescriptorSets();
    void createRenderPass();
    void createGraphicsPipeline();
    void createCommandBuffers();

    void update() override;

    void cleanup() override;
    void cleanupSwapChain() override;
    void recreateSwapChain() override;

// scene
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
    std::vector<std::shared_ptr<Buffer>> uniformBuffers;
};

} // namespace rfx::test