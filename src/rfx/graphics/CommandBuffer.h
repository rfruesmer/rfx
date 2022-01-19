#pragma once

#include "rfx/graphics/VertexBuffer.h"
#include "rfx/graphics/IndexBuffer.h"
#include "rfx/graphics/Image.h"

namespace rfx {

class CommandBuffer
{
public:
    CommandBuffer(VkDevice device, VkCommandBuffer commandBuffer);

    void begin() const;
    void begin(VkCommandBufferUsageFlags usage) const;
    void beginRenderPass(const VkRenderPassBeginInfo& beginInfo) const;
    void bindPipeline(const VkPipelineBindPoint& bindPoint, VkPipeline pipeline) const;
    void bindDescriptorSet(
        VkPipelineBindPoint bindPoint,
        VkPipelineLayout layout,
        uint32_t firstSet,
        VkDescriptorSet descriptorSet) const;
    void bindDescriptorSets(
        VkPipelineBindPoint bindPoint,
        VkPipelineLayout layout,
        const std::vector<VkDescriptorSet>& descriptorSets) const;
    void bindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) const;
    void bindVertexBuffers(const std::vector<std::shared_ptr<VertexBuffer>>& vertexBuffers) const;
    void bindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) const;
    void setViewport(const VkViewport& viewport);
    void setScissor(const VkRect2D& scissor);
    void draw(uint32_t vertexCount) const;
    void drawIndexed(uint32_t indexCount) const;
    void drawIndexed(uint32_t indexCount, uint32_t firstIndex) const;
    void endRenderPass() const;
    void end() const;

    void copyBuffer(
        const std::shared_ptr<Buffer>& sourceBuffer,
        const std::shared_ptr<Buffer>& destBuffer) const;

    void copyBufferToImage(
        const std::shared_ptr<Buffer>& buffer,
        const std::shared_ptr<Image>& image,
        const std::vector<VkBufferImageCopy>& regions) const;

    void setImageMemoryBarrier(
        const std::shared_ptr<Image>& image,
        VkAccessFlags srcAccess,
        VkAccessFlags dstAccess,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask) const;

    void setImageMemoryBarrier(
        const std::shared_ptr<Image>& image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT) const;

    void pushConstants(
        VkPipelineLayout layout,
        VkShaderStageFlags stageFlags,
        uint32_t offset,
        uint32_t size,
        const void* values);

    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

    void pipelineBarrier(
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkMemoryBarrier& outMemoryBarrier);

    [[nodiscard]] const VkCommandBuffer& getHandle() const;

private:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

using CommandBufferPtr = std::shared_ptr<CommandBuffer>;

} // namespace rfx