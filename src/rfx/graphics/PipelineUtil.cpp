#include "rfx/pch.h"
#include "rfx/graphics/PipelineUtil.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineLayout PipelineUtil::createPipelineLayout(
    const GraphicsDevicePtr& graphicsDevice,
    const vector<VkDescriptorSetLayout>& descriptorSetLayouts)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreatePipelineLayout(
        graphicsDevice->getLogicalDevice(),
        &pipelineLayoutInfo,
        nullptr,
        &pipelineLayout));

    return pipelineLayout;
}

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

VkPipeline PipelineUtil::createGraphicsPipeline(
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
    VkRenderPass renderPass)
{
    const array<VkPipelineShaderStageCreateInfo, 2> shaderStages {
        shaderProgram->getVertexShader()->getStageCreateInfo(),
        shaderProgram->getFragmentShader()->getStageCreateInfo()
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shaderStages.size(),
        .pStages = shaderStages.data(),
        .pVertexInputState = &shaderProgram->getVertexShader()->getVertexInputStateCreateInfo(),
        .pInputAssemblyState = &inputAssemblyState,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = &depthStencilState,
        .pColorBlendState = &colorBlendState,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1 // Optional
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateGraphicsPipelines(
        graphicsDevice->getLogicalDevice(),
        VK_NULL_HANDLE, // TODO: pipeline cache
        1,
        &pipelineCreateInfo,
        nullptr,
        &pipeline));


    // TODO: move wireframe pipeline creation to somewhere else
//    rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
//    ThrowIfFailed(vkCreateGraphicsPipelines(
//        graphicsDevice->getLogicalDevice(),
//        VK_NULL_HANDLE,
//        1,
//        &pipelineCreateInfo,
//        nullptr,
//        &wireframePipeline));

    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline PipelineUtil::createComputePipeline(
    const GraphicsDevicePtr& graphicsDevice,
    VkPipelineLayout pipelineLayout,
    const ComputeShaderPtr& computeShader)
{
    VkComputePipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = computeShader->getModule(),
            .pName = "main",
        },
        .layout = pipelineLayout
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    ThrowIfFailed(vkCreateComputePipelines(
        graphicsDevice->getLogicalDevice(),
        nullptr,
        1,
        &pipelineCreateInfo,
        nullptr,
        &pipeline));

    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------
