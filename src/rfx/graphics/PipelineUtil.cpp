#include "rfx/pch.h"
#include "rfx/graphics/PipelineUtil.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

VkPipelineInputAssemblyStateCreateInfo PipelineUtil::getDefaultInputAssemblyState()
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineRasterizationStateCreateInfo PipelineUtil::getDefaultRasterizationState()
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineColorBlendAttachmentState PipelineUtil::getDefaultColorBlendAttachmentState()
{
    return {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                        | VK_COLOR_COMPONENT_G_BIT
                        | VK_COLOR_COMPONENT_B_BIT
                        | VK_COLOR_COMPONENT_A_BIT
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineColorBlendStateCreateInfo PipelineUtil::getDefaultColorBlendState(
    const VkPipelineColorBlendAttachmentState* colorBlendAttachmentState)
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = colorBlendAttachmentState,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineDepthStencilStateCreateInfo PipelineUtil::getDefaultDepthStencilState()
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {}, // Optional
        .back = {}, // Optional
        .minDepthBounds = 0.0f, // Optional
        .maxDepthBounds = 1.0f, // Optional
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineViewportStateCreateInfo PipelineUtil::getDefaultViewportState()
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineMultisampleStateCreateInfo PipelineUtil::getDefaultMultisampleState(
    VkSampleCountFlagBits rasterizationSamples)
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = rasterizationSamples,
        .sampleShadingEnable = VK_FALSE
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineDynamicStateCreateInfo PipelineUtil::getDynamicState(const vector<VkDynamicState>& dynamicStates)
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };
}

// ---------------------------------------------------------------------------------------------------------------------
