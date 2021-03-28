#pragma once

#include "rfx/scene/Material.h"
#include "rfx/graphics/GraphicsDevice.h"
#include <rfx/graphics/VertexShader.h>
#include <rfx/graphics/FragmentShader.h>


namespace rfx {

class MaterialShader
{
public:
    virtual void create(
        const MaterialPtr& material,
        VkDescriptorSetLayout materialDescriptorSetLayout,
        const std::filesystem::path& shadersDirectory);

    void destroy();

    [[nodiscard]] const std::string& getId() const;
    [[nodiscard]] const std::shared_ptr<VertexShader>& getVertexShader() const;
    [[nodiscard]] const std::shared_ptr<FragmentShader>& getFragmentShader() const;

    [[nodiscard]] virtual std::vector<std::string> getShaderDefinesFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getVertexShaderInputsFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getVertexShaderOutputsFor(const MaterialPtr& material);
    [[nodiscard]] virtual std::vector<std::string> getFragmentShaderInputsFor(const MaterialPtr& material);

    void setPipeline(VkPipelineLayout pipelineLayout, VkPipeline pipeline);
    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;
    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkDescriptorSetLayout getMaterialDescriptorSetLayout() const;
    [[nodiscard]] virtual std::vector<std::byte> createDataFor(const MaterialPtr& material) const = 0;
    virtual void update(const MaterialPtr& material) const = 0;

    void setResources(
        VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet descriptorSet,
        BufferPtr buffer);
    [[nodiscard]] virtual const void* getData() const = 0;
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


    std::shared_ptr<GraphicsDevice> graphicsDevice_;

    std::string id;
    std::string vertexShaderId;
    std::shared_ptr<VertexShader> vertexShader;
    std::string fragmentShaderId;
    std::shared_ptr<FragmentShader> fragmentShader;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    VkDescriptorSetLayout materialDescriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorSetLayout shaderDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
    std::shared_ptr<Buffer> shaderDataBuffer;
};

using MaterialShaderPtr = std::shared_ptr<MaterialShader>;

} // namespace rfx