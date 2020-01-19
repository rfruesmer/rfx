#pragma once

#include "rfx/graphics/buffer/Buffer.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/shader/ShaderProgram.h"

namespace rfx
{

class Effect
{
public:
    Effect(const Effect&) = delete;
    Effect(Effect&&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&) = delete;
    virtual ~Effect();

    void dispose();

    void setModelMatrix(const glm::mat4& matrix);
    void setViewProjMatrix(const glm::mat4& matrix);
    void setModelViewProjMatrix(const glm::mat4& matrix) const;

    virtual const std::string& getId() const = 0;
    const std::shared_ptr<Buffer>& getUniformBuffer() const;
    const std::vector<VkDescriptorSet>& getDescriptorSets() const;
    const VertexFormat& getVertexFormat() const;
    VkPipeline getPipeline() const;
    VkPipelineLayout getPipelineLayout() const;

protected:
    explicit Effect(const std::shared_ptr<GraphicsDevice>& graphicsDevice, 
        VkRenderPass renderPass,
        std::unique_ptr<ShaderProgram>& shaderProgram);

    void initUniformBuffer(size_t size);
    void initDescriptorSetLayout(uint32_t bindingCount, const VkDescriptorSetLayoutBinding* bindings);
    void initDescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes);
    void initPipelineLayout();
    void initPipeline();

    virtual VkPipelineDynamicStateCreateInfo createDynamicState(
        uint32_t dynamicStateCount, VkDynamicState dynamicStates[]);
    virtual VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState();
    virtual VkPipelineRasterizationStateCreateInfo createRasterizationState();
    virtual VkPipelineColorBlendAttachmentState createColorBlendAttachmentState();
    virtual VkPipelineColorBlendStateCreateInfo createColorBlendState(
        const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState);
    virtual VkPipelineViewportStateCreateInfo createViewportState();
    virtual VkPipelineDepthStencilStateCreateInfo createDepthStencilState();
    virtual VkPipelineMultisampleStateCreateInfo createMultiSampleState();

    std::shared_ptr<GraphicsDevice> graphicsDevice;
    VkDescriptorPool descriptorPool = nullptr;
    VkRenderPass renderPass = nullptr;
    std::unique_ptr<ShaderProgram> shaderProgram;

    VkPipelineLayout pipelineLayout = nullptr;
    VkPipeline pipeline = nullptr;

    std::shared_ptr<Buffer> uniformBuffer;
    VkDescriptorSetLayout descriptorSetLayout = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewProjMatrix = glm::mat4(1.0f);
};
    
}