#include "rfx/pch.h"
#include "rfx/rendering/RenderGraph.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

RenderGraph::RenderGraph(
    GraphicsDevicePtr graphicsDevice,
    VkDescriptorSet sceneDescriptorSet)
        : graphicsDevice(move(graphicsDevice)),
          sceneDescriptorSet(sceneDescriptorSet) {}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(
    const ModelPtr& model,
    const unordered_map<MaterialShaderPtr, vector<MaterialPtr>>& materialShaderMap)
{
    for (const auto& [shader, materials] : materialShaderMap) {
        add(shader, materials, model);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(
    const MaterialShaderPtr& shader,
    const vector<MaterialPtr>& materials,
    const ModelPtr& model)
{
    ShaderNode childNode(shader, materials, model, sceneDescriptorSet);
    childNodeMap[model].push_back(childNode);
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::add(RenderGraphNodePtr userDefinedNode)
{
    userDefinedNodes.push_back(move(userDefinedNode));
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::record(
    const CommandBufferPtr& commandBuffer,
    VkRenderPass renderPass,
    VkFramebuffer renderTarget)
{
    begin(commandBuffer, renderPass, renderTarget);

    setViewportAndScissor(commandBuffer);

    for (const auto& userDefinedNode : userDefinedNodes) {
        if (userDefinedNode->isEnabled()) {
            userDefinedNode->record(commandBuffer);
        }
    }

    for (const auto& [model, shaderNodes] : childNodeMap)
    {
        bindGeometryBuffers(commandBuffer, model);
        recordShaderNodes(shaderNodes, commandBuffer);
    }

    end(commandBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::begin(
    const CommandBufferPtr& commandBuffer,
    VkRenderPass renderPass,
    VkFramebuffer renderTarget)
{
    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();
    const unique_ptr<DepthBuffer>& depthBuffer = graphicsDevice->getDepthBuffer();

    vector<VkClearValue> clearValues(1);
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    if (graphicsDevice->getMultiSampleCount() > VK_SAMPLE_COUNT_1_BIT) {
        clearValues.resize(clearValues.size() + 1);
        clearValues[clearValues.size() - 1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    }
    if (depthBuffer) {
        clearValues.resize(clearValues.size() + 1);
        clearValues[clearValues.size() - 1].depthStencil = { 1.0f, 0 };
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = renderTarget,
        .renderArea = {
            .offset = { 0, 0 },
            .extent = swapChainDesc.extent
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()
    };

    commandBuffer->begin();
    commandBuffer->beginRenderPass(renderPassBeginInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::setViewportAndScissor(const CommandBufferPtr& commandBuffer) const
{
    const unique_ptr<SwapChain>& swapChain = graphicsDevice->getSwapChain();
    const SwapChainDesc& swapChainDesc = swapChain->getDesc();

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapChainDesc.extent.width),
        .height = static_cast<float>(swapChainDesc.extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = swapChainDesc.extent
    };

    commandBuffer->setViewport(viewport);
    commandBuffer->setScissor(scissor);
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::bindGeometryBuffers(
    const CommandBufferPtr& commandBuffer,
    const ModelPtr& model)
{
    commandBuffer->bindVertexBuffer(model->getVertexBuffer());
    commandBuffer->bindIndexBuffer(model->getIndexBuffer());
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::recordShaderNodes(
    const vector<ShaderNode>& shaderNodes,
    const CommandBufferPtr& commandBuffer)
{
    for (const auto& shaderNode : shaderNodes) {
        shaderNode.record(commandBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void RenderGraph::end(const CommandBufferPtr& commandBuffer)
{
    commandBuffer->endRenderPass();
    commandBuffer->end();
}

// ---------------------------------------------------------------------------------------------------------------------
