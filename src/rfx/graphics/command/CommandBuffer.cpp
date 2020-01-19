#include "rfx/pch.h"
#include "rfx/graphics/command/CommandBuffer.h"
#include "rfx/core/Algorithm.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

CommandBuffer::CommandBuffer(
    VkCommandBuffer vkCommandBuffer, 
    const VulkanDeviceFunctionPtrs& vk)
        : vkCommandBuffer(vkCommandBuffer),
          vkBeginCommandBuffer(vk.vkBeginCommandBuffer),
          vkEndCommandBuffer(vk.vkEndCommandBuffer),
          vkCmdBeginRenderPass(vk.vkCmdBeginRenderPass),
          vkCmdEndRenderPass(vk.vkCmdEndRenderPass),
          vkCmdBindVertexBuffers(vk.vkCmdBindVertexBuffers),
          vkCmdBindIndexBuffer(vk.vkCmdBindIndexBuffer),
          vkCmdBindPipeline(vk.vkCmdBindPipeline),
          vkCmdBindDescriptorSets(vk.vkCmdBindDescriptorSets),
          vkCmdSetViewport(vk.vkCmdSetViewport),
          vkCmdSetScissor(vk.vkCmdSetScissor),
          vkCmdDraw(vk.vkCmdDraw),
          vkCmdDrawIndexed(vk.vkCmdDrawIndexed),
          vkCmdCopyBuffer(vk.vkCmdCopyBuffer),
          vkCmdCopyBufferToImage(vk.vkCmdCopyBufferToImage),
          vkCmdPipelineBarrier(vk.vkCmdPipelineBarrier) {}

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

void CommandBuffer::bindIndexBuffer(const shared_ptr<Buffer>& buffer)
{
    vkCmdBindIndexBuffer(vkCommandBuffer, buffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);
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

void CommandBuffer::drawIndexed(uint32_t indexCount) const
{
    vkCmdDrawIndexed(vkCommandBuffer, indexCount, 1, 0, 0, 0);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::copyBuffer(
    const std::shared_ptr<Buffer>& sourceBuffer, 
    const std::shared_ptr<Buffer>& destBuffer)
{
    VkBufferCopy copyRegion = {};
    copyRegion.size = sourceBuffer->getBufferInfo().range;

    vkCmdCopyBuffer(vkCommandBuffer, sourceBuffer->getHandle(), destBuffer->getHandle(), 1, &copyRegion);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::copyBufferToImage(VkBuffer buffer, const shared_ptr<Image>& image) const
{
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { image->getWidth(), image->getHeight(), 1 };

    vkCmdCopyBufferToImage(vkCommandBuffer, buffer, image->getHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::setImageMemoryBarrier(
    const shared_ptr<Image>& image, 
    VkAccessFlags sourceAccess, 
    VkAccessFlags destAccess,
    VkImageLayout oldLayout, 
    VkImageLayout newLayout, 
    VkPipelineStageFlags sourceStage,
    VkPipelineStageFlags destinationStage) const
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = sourceAccess;
    barrier.dstAccessMask = destAccess;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image->getHandle();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdPipelineBarrier(
        vkCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

// ---------------------------------------------------------------------------------------------------------------------

VkCommandBuffer CommandBuffer::getHandle() const
{
    return vkCommandBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::begin(VkCommandBufferUsageFlags usage) const
{
    RFX_CHECK_STATE(vkCommandBuffer != nullptr,
        "Invalid command buffer");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = usage;
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
