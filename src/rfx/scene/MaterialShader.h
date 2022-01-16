#pragma once

#include "rfx/scene/Material.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/ShaderProgram.h"

namespace rfx {

class MaterialShader
{
public:
    void create(
        ShaderProgramPtr shaderProgram,
        VkDescriptorSetLayout shaderDescriptorSetLayout,
        VkDescriptorSet shaderDescriptorSet,
        BufferPtr shaderDataBuffer,
        VkDescriptorSetLayout materialDescriptorSetLayout);

    void destroy();

    [[nodiscard]] const std::string& getId() const;
    [[nodiscard]] const std::string& getVertexShaderId() const;
    [[nodiscard]] const std::string& getFragmentShaderId() const;
    [[nodiscard]] const ShaderProgramPtr& getShaderProgram() const;

    [[nodiscard]] virtual std::vector<std::string> getShaderDefinesFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getVertexShaderInputsFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getVertexShaderOutputsFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getFragmentShaderInputsFor(const MaterialPtr& material);

    void setPipeline(VkPipelineLayout pipelineLayout, VkPipeline pipeline);
    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;
    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkDescriptorSetLayout getMaterialDescriptorSetLayout() const;
    [[nodiscard]] virtual std::vector<std::byte> createDataFor(const MaterialPtr& material) const = 0;
    virtual void update(const MaterialPtr& material) const;

    // TODO: rename to getShaderData
    [[nodiscard]] virtual const void* getData() const = 0;

    // TODO: rename to getShaderDataSize
    [[nodiscard]] virtual uint32_t getDataSize() const = 0;
    [[nodiscard]] VkDescriptorSetLayout getShaderDescriptorSetLayout() const;
    [[nodiscard]] VkDescriptorSet getShaderDescriptorSet() const;
    void updateDataBuffer();

protected:
    MaterialShader(
        GraphicsDevicePtr graphicsDevice,
        std::string id,
        std::string vertexShaderId,
        std::string fragmentShaderId);
    virtual ~MaterialShader();


    GraphicsDevicePtr graphicsDevice;

    std::string id;
    std::string vertexShaderId;
    std::string fragmentShaderId;
    ShaderProgramPtr shaderProgram;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    VkDescriptorSetLayout materialDescriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorSetLayout shaderDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
    BufferPtr shaderDataBuffer;
};

using MaterialShaderPtr = std::shared_ptr<MaterialShader>;

} // namespace rfx