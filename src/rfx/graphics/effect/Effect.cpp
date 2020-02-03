#include "rfx/pch.h"
#include "rfx/graphics/effect/Effect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

Effect::Effect(const shared_ptr<GraphicsDevice>& graphicsDevice,
               VkRenderPass renderPass,
               unique_ptr<ShaderProgram>& shaderProgram)
        : graphicsDevice(graphicsDevice),
          renderPass(renderPass),
          shaderProgram(move(shaderProgram))
{
    RFX_CHECK_ARGUMENT(graphicsDevice != nullptr);
    RFX_CHECK_ARGUMENT(renderPass != nullptr);
    RFX_CHECK_ARGUMENT(this->shaderProgram != nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::init()
{
    createUniformBuffers();
    createDescriptorSetLayout();
    createDescriptorPool();
    initDescriptorSets();
    createPipelineLayout();
    createPipeline();
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::createUniformBuffer(size_t size)
{
    const shared_ptr<Buffer> uniformBuffer = graphicsDevice->createUniformBuffer(size);
    uniformBuffer->bind();
    uniformBuffers.push_back(uniformBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::initDescriptorSets()
{
    allocateDescriptorSets();
    updateDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::allocateDescriptorSets()
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    graphicsDevice->allocateDescriptorSets(descriptorSetAllocateInfo, descriptorSets);
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::createDescriptorSetLayout(const vector<VkDescriptorSetLayoutBinding>& bindings)
{
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    descriptorSetLayout = graphicsDevice->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSetLayoutBinding Effect::createDescriptorSetLayoutBinding(
    uint32_t index, 
    VkDescriptorType type,
    VkShaderStageFlagBits stageFlags)
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = index;
    layoutBinding.descriptorType = type;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = nullptr;

    return layoutBinding;
}

// ---------------------------------------------------------------------------------------------------------------------

VkWriteDescriptorSet Effect::createDescriptorWrite(
    uint32_t index, 
    VkDescriptorSet descriptorSet, 
    VkDescriptorType descriptorType,
    const VkDescriptorBufferInfo& descriptorBufferInfo)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = index;
    write.dstSet = descriptorSet;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = descriptorType;
    write.pBufferInfo = &descriptorBufferInfo;

    return write;
}

// ---------------------------------------------------------------------------------------------------------------------

VkWriteDescriptorSet Effect::createDescriptorWrite(
    uint32_t index, 
    VkDescriptorSet descriptorSet,
    VkDescriptorType descriptorType, 
    const VkDescriptorImageInfo& descriptorImageInfo)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstBinding = index;
    write.dstSet = descriptorSet;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = descriptorType;
    write.pImageInfo = &descriptorImageInfo;

    return write;
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::createDescriptorPool(const std::vector<VkDescriptorPoolSize>& poolSizes)
{
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

    descriptorPool = graphicsDevice->createDescriptorPool(descriptorPoolCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

Effect::~Effect()
{
    dispose();
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::dispose()
{
    for (const auto& uniformBuffer : uniformBuffers) {
        uniformBuffer->dispose();
    }
    uniformBuffers.clear();

    if (pipeline) {
        graphicsDevice->destroyPipeline(pipeline);
        pipeline = nullptr;
    }

    if (pipelineLayout) {
        graphicsDevice->destroyPipelineLayout(pipelineLayout);
        pipelineLayout = nullptr;
    }

    if (descriptorPool) {
        graphicsDevice->destroyDescriptorPool(descriptorPool);
        descriptorPool = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::createPipelineLayout()
{
    RFX_CHECK_STATE(descriptorSetLayout != nullptr, "descriptorSetLayout must be created before");

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

    pipelineLayout = graphicsDevice->createPipelineLayout(pipelineLayoutCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::createPipeline()
{
    RFX_CHECK_STATE(pipelineLayout != nullptr, "Render pass must be setup before");
    RFX_CHECK_STATE(renderPass != nullptr, "Render pass must be setup before");

    VkDynamicState dynamicStateEnables[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = createDynamicState(2, dynamicStateEnables);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = createInputAssemblyState();
    VkPipelineRasterizationStateCreateInfo rasterizationState = createRasterizationState();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = createColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = createColorBlendState(colorBlendAttachmentState);
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = createViewportState();
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = createDepthStencilState();
    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = createMultiSampleState();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = nullptr;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pVertexInputState = &shaderProgram->getVertexShader()->getVertexInputState();
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderProgram->getShaderStages().size());
    pipelineCreateInfo.pStages = shaderProgram->getShaderStages().data();
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipeline = graphicsDevice->createGraphicsPipeline(pipelineCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineDynamicStateCreateInfo Effect::createDynamicState(uint32_t dynamicStateCount, VkDynamicState dynamicStates[])
{
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.dynamicStateCount = dynamicStateCount;
    dynamicState.pDynamicStates = dynamicStates;

    return dynamicState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineInputAssemblyStateCreateInfo Effect::createInputAssemblyState()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    return inputAssemblyState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineRasterizationStateCreateInfo Effect::createRasterizationState()
{
    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.pNext = nullptr;
    rasterizationState.flags = 0;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthBiasConstantFactor = 0;
    rasterizationState.depthBiasClamp = 0;
    rasterizationState.depthBiasSlopeFactor = 0;
    rasterizationState.lineWidth = 1.0F;

    return rasterizationState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineColorBlendAttachmentState Effect::createColorBlendAttachmentState()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = 0xf;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    return colorBlendAttachmentState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineColorBlendStateCreateInfo Effect::createColorBlendState(
    const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState)
{
    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.flags = 0;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendState.blendConstants[0] = 1.0F;
    colorBlendState.blendConstants[1] = 1.0F;
    colorBlendState.blendConstants[2] = 1.0F;
    colorBlendState.blendConstants[3] = 1.0F;

    return colorBlendState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineViewportStateCreateInfo Effect::createViewportState()
{
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;
    viewportState.pViewports = nullptr;

    return viewportState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineDepthStencilStateCreateInfo Effect::createDepthStencilState()
{
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.pNext = nullptr;
    depthStencilState.flags = 0;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0;
    depthStencilState.maxDepthBounds = 0;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.back.compareMask = 0;
    depthStencilState.back.reference = 0;
    depthStencilState.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.writeMask = 0;
    depthStencilState.front = depthStencilState.back;

    return depthStencilState;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineMultisampleStateCreateInfo Effect::createMultiSampleState()
{
    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = {};
    multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleStateCreateInfo.pNext = nullptr;
    multiSampleStateCreateInfo.flags = 0;
    multiSampleStateCreateInfo.pSampleMask = nullptr;
    multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multiSampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multiSampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    multiSampleStateCreateInfo.minSampleShading = 0.0;

    return multiSampleStateCreateInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::setModelMatrix(const mat4& matrix)
{
    modelMatrix = matrix;
    setModelViewProjMatrix(viewProjMatrix * modelMatrix);
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::setViewProjMatrix(const mat4& matrix)
{
    viewProjMatrix = matrix;
    setModelViewProjMatrix(viewProjMatrix * modelMatrix);
}

// ---------------------------------------------------------------------------------------------------------------------

void Effect::updateFrom(const shared_ptr<Camera>& camera)
{
    setViewProjMatrix(camera->getViewProjMatrix());
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& Effect::getDescriptorSets() const
{
    return descriptorSets;
}

// ---------------------------------------------------------------------------------------------------------------------

const VertexFormat& Effect::getVertexFormat() const
{
    return shaderProgram->getVertexShader()->getVertexFormat();
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipeline Effect::getPipeline() const
{
    return pipeline;
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineLayout Effect::getPipelineLayout() const
{
    return pipelineLayout;
}

// ---------------------------------------------------------------------------------------------------------------------
