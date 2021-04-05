#pragma once


namespace rfx {

class PipelineUtil
{
public:
    PipelineUtil() = delete;

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
};

} // namespace rfx
