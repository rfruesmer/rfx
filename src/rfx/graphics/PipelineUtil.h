#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/ShaderProgram.h"
#include "rfx/graphics/ComputeShader.h"


namespace rfx {

class PipelineUtil
{
public:
    PipelineUtil() = delete;

    static VkPipelineLayout createPipelineLayout(
        const GraphicsDevicePtr& graphicsDevice,
        const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);

    static VkPipelineInputAssemblyStateCreateInfo getDefaultInputAssemblyState();
    static VkPipelineRasterizationStateCreateInfo getDefaultRasterizationState();
    static VkPipelineColorBlendAttachmentState getDefaultColorBlendAttachmentState();
    static VkPipelineColorBlendStateCreateInfo getDefaultColorBlendState(
        const VkPipelineColorBlendAttachmentState* colorBlendAttachmentState);
    static VkPipelineDepthStencilStateCreateInfo getDefaultDepthStencilState();
    static VkPipelineViewportStateCreateInfo getDefaultViewportState();
    static VkPipelineMultisampleStateCreateInfo getDefaultMultisampleState(
        VkSampleCountFlagBits rasterizationSamples);
    static VkPipelineDynamicStateCreateInfo getDynamicState(const std::vector<VkDynamicState>& dynamicStates);

    static VkPipeline createGraphicsPipeline(
        const GraphicsDevicePtr& graphicsDevice,
        VkPipelineLayout pipelineLayout,
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState,
        VkPipelineRasterizationStateCreateInfo rasterizationState,
        VkPipelineColorBlendStateCreateInfo colorBlendState,
        VkPipelineDepthStencilStateCreateInfo depthStencilState,
        VkPipelineViewportStateCreateInfo viewportState,
        VkPipelineMultisampleStateCreateInfo multisampleState,
        VkPipelineDynamicStateCreateInfo dynamicState,
        const ShaderProgramPtr& shaderProgram,
        VkRenderPass renderPass);

    static VkPipeline createComputePipeline(
        const GraphicsDevicePtr& graphicsDevice,
        VkPipelineLayout pipelineLayout,
        const ComputeShaderPtr& computeShader);
};

} // namespace rfx
