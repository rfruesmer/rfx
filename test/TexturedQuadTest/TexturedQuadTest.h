#pragma once


#include "rfx/application/Application.h"

namespace rfx::test {

class TexturedQuadTest : public Application
{
private:
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        float lodBias = 0.0f;
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
    void createTexture();

    void update(float deltaTime) override;
    void updateDevTools() override;

    void cleanup() override;
    void cleanupSwapChain() override;
    void recreateSwapChain() override;


    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VertexBufferPtr vertexBuffer;
    IndexBufferPtr indexBuffer;
    VertexShaderPtr vertexShader;
    FragmentShaderPtr fragmentShader;
    std::vector<BufferPtr> uniformBuffers;
    Texture2DPtr texture;
    UniformBufferObject ubo;
};

} // namespace rfx
