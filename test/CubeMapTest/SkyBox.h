#pragma once

#include "rfx/scene/Model.h"
#include "rfx/scene/Camera.h"
#include "rfx/graphics/ShaderProgram.h"

namespace rfx {

class SkyBox
{
public:
    SkyBox(
        GraphicsDevicePtr graphicsDevice,
        VkDescriptorPool descriptorPool);

    ~SkyBox();

    void create(
        const std::filesystem::path& modelPath,
        const std::filesystem::path& cubeMapPath,
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath,
        VkRenderPass renderPass);

    [[nodiscard]] const ModelPtr& getModel() const;
    [[nodiscard]] VkDescriptorSet getDescriptorSet() const;
    [[nodiscard]] VkPipeline getPipeline() const;
    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

    void updateUniformBuffer(const CameraPtr& camera);

    void cleanupSwapChain();

private:
    struct ShaderData
    {
        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;
    };

    void loadModel(const std::filesystem::path& modelPath);
    void loadCubeMap(const std::filesystem::path& cubeMapPath);
    void loadShaderProgram(
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath);
    void createUniformBuffer();
    void createDescriptorSetLayout();
    void createDescriptorSet();
    void createPipeline(VkRenderPass renderPass);

    GraphicsDevicePtr graphicsDevice;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    ModelPtr model; // TODO: merge into scene vertex- & index-buffer
    CubeMapPtr cubeMap;
    ShaderProgramPtr shaderProgram;
    BufferPtr uniformBuffer;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};

using SkyBoxPtr = std::shared_ptr<SkyBox>;

} // namespace rfx
