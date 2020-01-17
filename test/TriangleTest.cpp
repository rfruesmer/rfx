#include "rfx/pch.h"
#include "test/TriangleTest.h"
#include "rfx/graphics/ShaderLoader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

TriangleTest::TriangleTest(handle_t instanceHandle)
    : TestApplication("assets/tests/triangle/application-config.json", instanceHandle) {}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initialize()
{
    Application::initialize();

    initCommandPool();
    initRenderPass();
    initFrameBuffers();

    initScene();
    initCamera();

    initDescriptorSetLayout();
    initPipelineLayout();
    initPipeline();
    initDescriptorPool();
    initDescriptorSet();
    initCommandBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initScene()
{
    const uint32_t vertexCount = 3;
    const VertexFormat vertexFormat(VertexFormat::COORDINATES | VertexFormat::COLORS);
    const uint32_t vertexBufferSize = vertexCount * vertexFormat.getVertexSize();
    vector<float> vertexData = {
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
    };

    const shared_ptr<Buffer> stagingVertexBuffer = graphicsDevice->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingVertexBuffer->load(vertexBufferSize, 
        reinterpret_cast<const std::byte*>(vertexData.data()));
    stagingVertexBuffer->bind();

    vertexBuffer = graphicsDevice->createVertexBuffer(vertexCount, vertexFormat);
    vertexBuffer->bind();


    const uint32_t indexCount = 3;
    const uint32_t indexBufferSize = indexCount * sizeof(uint32_t);
    const vector<uint32_t> indexData = { 0, 1, 2 };

    const shared_ptr<Buffer> stagingIndexBuffer = graphicsDevice->createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingIndexBuffer->load(indexBufferSize, 
        reinterpret_cast<const std::byte*>(indexData.data()));
    stagingIndexBuffer->bind();

    indexBuffer = graphicsDevice->createIndexBuffer(indexCount, VK_INDEX_TYPE_UINT32);
    indexBuffer->bind();

    const shared_ptr<CommandPool>& commandPool = graphicsDevice->getTempCommandPool();
    shared_ptr<CommandBuffer> commandBuffer = commandPool->allocateCommandBuffer();
    commandBuffer->begin();
    commandBuffer->copyBuffer(stagingVertexBuffer, vertexBuffer);
    commandBuffer->copyBuffer(stagingIndexBuffer, indexBuffer);
    commandBuffer->end();

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence = graphicsDevice->createFence(fenceCreateInfo);

    const shared_ptr<Queue>& queue = graphicsDevice->getGraphicsQueue();
    queue->submit(commandBuffer, fence);

    const VkResult result = graphicsDevice->waitForFences(1, &fence, true, DEFAULT_FENCE_TIMEOUT);
    RFX_CHECK_STATE(result == VK_SUCCESS, "failed to submit copy commands");

    graphicsDevice->destroyFence(fence);

    commandPool->freeCommandBuffer(commandBuffer);

    ShaderLoader shaderLoader(graphicsDevice);
    shaderStages[0] = shaderLoader.load("assets/common/shaders/color.vert", VK_SHADER_STAGE_VERTEX_BIT, "main");
    shaderStages[1] = shaderLoader.load("assets/common/shaders/color.frag", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initPipeline()
{
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
    pipelineCreateInfo.pVertexInputState = &vertexBuffer->getInputState();
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipeline = graphicsDevice->createGraphicsPipeline(pipelineCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initDescriptorPool()
{
    TestApplication::initDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initDescriptorSet()
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = NUM_DESCRIPTOR_SETS;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    graphicsDevice->allocateDescriptorSets(descriptorSetAllocateInfo, descriptorSets);

    VkWriteDescriptorSet writes = {};
    writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes.pNext = nullptr;
    writes.dstSet = descriptorSets[0];
    writes.descriptorCount = 1;
    writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes.pBufferInfo = &uniformBuffer->getBufferInfo();
    writes.dstArrayElement = 0;
    writes.dstBinding = 0;

    graphicsDevice->updateDescriptorSets(1, &writes);
}

// ---------------------------------------------------------------------------------------------------------------------

void TriangleTest::initCommandBuffers()
{
    drawCommandBuffers = commandPool->allocateCommandBuffers(graphicsDevice->getSwapChainBuffers().size());

    const VkExtent2D presentImageSize = graphicsDevice->getSwapChainProperties().imageSize;

    VkClearValue clearValues[2];
    clearValues[0].color = { 0.05f, 0.05f, 0.05f, 1.0f };
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(presentImageSize.width);
    viewport.height = static_cast<float>(presentImageSize.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.extent.width = presentImageSize.width;
    scissor.extent.height = presentImageSize.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    for (size_t i = 0, count = drawCommandBuffers.size(); i < count; ++i) {
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = frameBuffers[i];
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent = presentImageSize;
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        auto& commandBuffer = drawCommandBuffers[i];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        commandBuffer->setViewport(viewport);
        commandBuffer->setScissor(scissor);
        commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, descriptorSets);
        commandBuffer->bindVertexBuffers({ vertexBuffer });
        commandBuffer->bindIndexBuffer(indexBuffer);
        commandBuffer->drawIndexed(indexBuffer->getIndexCount());
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }

}

// ---------------------------------------------------------------------------------------------------------------------
