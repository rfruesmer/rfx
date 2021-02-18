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
    void bindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout layout, VkDescriptorSet descriptorSet) const;
    void bindVertexBuffers(const std::vector<std::shared_ptr<VertexBuffer>>& vertexBuffers) const;
    void bindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) const;
    void draw(uint32_t vertexCount) const;
    void drawIndexed(uint32_t indexCount) const;
    void endRenderPass() const;
    void end() const;

    void copyBuffer(
        const std::shared_ptr<Buffer>& sourceBuffer,
        const std::shared_ptr<Buffer>& destBuffer) const;
    void copyBufferToImage(
        const std::shared_ptr<Buffer>& buffer,
        const std::shared_ptr<Image>& image) const;


    void setImageMemoryBarrier(
        const std::shared_ptr<Image>& image,
        VkAccessFlags sourceAccess,
        VkAccessFlags destAccess,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkPipelineStageFlags sourceStage,
        VkPipelineStageFlags destinationStage) const;

    [[nodiscard]] const VkCommandBuffer& getHandle() const;

private:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

} // namespace rfx