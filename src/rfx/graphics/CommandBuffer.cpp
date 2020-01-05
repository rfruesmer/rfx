#include "rfx/pch.h"
#include "rfx/graphics/CommandBuffer.h"
#include "rfx/core/Algorithm.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

CommandBuffer::CommandBuffer(
    VkCommandBuffer vkCommandBuffer, 
    const VulkanDeviceFunctionPtrs& vk)
        : vkCommandBuffer(vkCommandBuffer)
{
    vkBeginCommandBuffer = vk.vkBeginCommandBuffer;
    vkEndCommandBuffer = vk.vkEndCommandBuffer;
    vkCmdBeginRenderPass = vk.vkCmdBeginRenderPass;
    vkCmdEndRenderPass = vk.vkCmdEndRenderPass;
    vkCmdBindVertexBuffers = vk.vkCmdBindVertexBuffers;
    vkCmdBindPipeline = vk.vkCmdBindPipeline;
    vkCmdBindDescriptorSets = vk.vkCmdBindDescriptorSets;
    vkCmdSetViewport = vk.vkCmdSetViewport;
    vkCmdSetScissor = vk.vkCmdSetScissor;
    vkCmdDraw = vk.vkCmdDraw;
}

// ---------------------------------------------------------------------------------------------------------------------

CommandBuffer::~CommandBuffer()
{
    int i = 42;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::invalidate()
{
    vkCommandBuffer = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

bool CommandBuffer::isValid() const
{
    return vkCommandBuffer != nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::beginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents) const
{
    vkCmdBeginRenderPass(vkCommandBuffer, &beginInfo, contents);   
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::endRenderPass() const
{
    vkCmdEndRenderPass(vkCommandBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) const
{
     vkCmdBindPipeline(vkCommandBuffer, bindPoint, pipeline);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindVertexBuffers(const vector<shared_ptr<Buffer>>& buffers) const
{
    vector<VkBuffer> bufferHandles;
    bufferHandles.reserve(buffers.size());

    rfx::transform(buffers, back_inserter(bufferHandles),
        [](const shared_ptr<Buffer>& buffer) -> VkBuffer { return buffer->getHandle(); });

    const VkDeviceSize offsets[1] = {0}; // TODO: support for multiple buffers
    vkCmdBindVertexBuffers(vkCommandBuffer, 0, static_cast<uint32_t>(buffers.size()), bufferHandles.data(), offsets);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint bindPoint, 
    VkPipelineLayout pipelineLayout,
    const vector<VkDescriptorSet>& descriptorSets) const
{
    vkCmdBindDescriptorSets(vkCommandBuffer, bindPoint, pipelineLayout, 0, 
        static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::setViewport(const VkViewport& viewport) const
{
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::setScissor(const VkRect2D& scissor) const
{
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::draw(uint32_t vertexCount) const
{
    vkCmdDraw(vkCommandBuffer, vertexCount, 1, 0, 0);
}

// ---------------------------------------------------------------------------------------------------------------------

VkCommandBuffer CommandBuffer::getHandle() const
{
    return vkCommandBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::begin() const
{
    RFX_CHECK_STATE(vkCommandBuffer != nullptr,
        "Invalid command buffer");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    const VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to begin command buffer recording");
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::end() const
{
    const VkResult result = vkEndCommandBuffer(vkCommandBuffer);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to stop command buffer recording");
}

// ---------------------------------------------------------------------------------------------------------------------
