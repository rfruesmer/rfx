#include "rfx/pch.h"
#include "rfx/graphics/CommandBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

CommandBuffer::CommandBuffer(VkDevice device, VkCommandBuffer commandBuffer)
    : device(device),
      commandBuffer(commandBuffer) {}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::begin() const
{
    begin(0);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::begin(VkCommandBufferUsageFlags usage) const
{
    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usage,
        .pInheritanceInfo = nullptr // Optional
    };

    ThrowIfFailed(vkBeginCommandBuffer(
        commandBuffer,
        &beginInfo));
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::beginRenderPass(const VkRenderPassBeginInfo& beginInfo) const
{
    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindPipeline(const VkPipelineBindPoint& bindPoint, VkPipeline pipeline) const
{
    vkCmdBindPipeline(commandBuffer, bindPoint, pipeline);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindDescriptorSet(
    VkPipelineBindPoint bindPoint,
    VkPipelineLayout layout,
    VkDescriptorSet descriptorSet) const
{
    vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, 0, 1, &descriptorSet, 0, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindVertexBuffers(const vector<shared_ptr<VertexBuffer>>& vertexBuffers) const
{
    if (vertexBuffers.size() > 1) { // TODO: support for multiple buffers
        RFX_THROW_NOT_IMPLEMENTED();
    }

    vector<VkBuffer> vertexBufferHandles;
    vertexBufferHandles.reserve(vertexBuffers.size());
    ranges::transform(vertexBuffers, back_inserter(vertexBufferHandles),
        [](const shared_ptr<VertexBuffer>& vertexBuffer) { return vertexBuffer->getHandle(); });

    const VkDeviceSize offsets[] = { 0 }; // TODO: support for multiple buffers
    vkCmdBindVertexBuffers(
        commandBuffer,
        0,
        static_cast<uint32_t>(vertexBuffers.size()),
        vertexBufferHandles.data(),
        offsets);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::bindIndexBuffer(const shared_ptr<IndexBuffer>& indexBuffer) const
{
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getHandle(), 0, indexBuffer->getIndexType());
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::draw(uint32_t vertexCount) const
{
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::drawIndexed(uint32_t indexCount) const
{
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::endRenderPass() const
{
    vkCmdEndRenderPass(commandBuffer);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::end() const
{
    ThrowIfFailed(vkEndCommandBuffer(commandBuffer));
}

// ---------------------------------------------------------------------------------------------------------------------

const VkCommandBuffer& CommandBuffer::getHandle() const
{
    return commandBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::copyBuffer(
    const shared_ptr<Buffer>& sourceBuffer,
    const shared_ptr<Buffer>& destBuffer) const
{
    VkBufferCopy copyRegion {
        .srcOffset = 0, // Optional
        .dstOffset = 0, // Optional
        .size = sourceBuffer->getSize()
    };

    vkCmdCopyBuffer(commandBuffer, sourceBuffer->getHandle(), destBuffer->getHandle(), 1, &copyRegion);
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::copyBufferToImage(
    const shared_ptr<Buffer>& buffer,
    const shared_ptr<Image>& image,
    const std::vector<VkBufferImageCopy>& regions) const
{
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer->getHandle(),
        image->getHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        regions.size(),
        regions.data());
}

// ---------------------------------------------------------------------------------------------------------------------

void CommandBuffer::setImageMemoryBarrier(
    const shared_ptr<Image>& image,
    VkAccessFlags srcAccess,
    VkAccessFlags dstAccess,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask) const
{
    VkImageMemoryBarrier barrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = srcAccess,
        .dstAccessMask = dstAccess,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image->getHandle(),
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = image->getDesc().mipLevels,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );
}

// ---------------------------------------------------------------------------------------------------------------------

