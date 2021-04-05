#pragma once

#include "TestApplication.h"
#include "SkyBoxShader.h"


namespace rfx {

class CubeMapTest : public TestApplication
{
protected:
    void initGraphics() override;
    void initShaderFactory(MaterialShaderFactory& shaderFactory) override;
    void createMeshResources() override;
    void updateShaderData() override;
    void updateDevTools() override;
    void cleanup() override;

    void cleanupSwapChain() override;

    void createSceneResources() override;
    void updateSceneData() override;

private:
    struct SkyBoxData
    {
        glm::mat4 projMatrix;
        glm::mat4 modelMatrix;
    };

    void loadScene();
    void buildRenderGraph();

    void createSkyboxResources();
    void createSkyboxDataBuffer();
    void createSkyboxDescriptorSetLayout();
    void createSkyboxDescriptorSet();

protected:
    void createPipelines() override;

    void createCommandBuffers() override;

private:
    ModelPtr skyBoxModel;
    std::shared_ptr<SkyBoxShader> skyBoxShader;
    ShaderProgramPtr skyBoxShaderProgram;
    CubeMapPtr skyBoxCubeMap;
    BufferPtr skyBoxDataBuffer;
    SkyBoxData skyBoxData {};
    VkDescriptorSetLayout skyBoxDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet skyBoxDescriptorSet = VK_NULL_HANDLE;
    VkPipeline skyBoxPipeline = VK_NULL_HANDLE;
    VkPipelineLayout skyBoxPipelineLayout = VK_NULL_HANDLE;
};

} // namespace rfx
