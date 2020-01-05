#pragma once

#include "rfx/graphics/Buffer.h"

namespace rfx
{

class CommandBuffer
{
public:
    explicit CommandBuffer(VkCommandBuffer vkCommandBuffer,
        const VulkanDeviceFunctionPtrs& vk);
    ~CommandBuffer();

    VkCommandBuffer getHandle() const;

    void begin() const;
    void end() const;

    void invalidate();
    bool isValid() const;

    void beginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents) const;
    void endRenderPass() const;
    void bindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) const;
    void bindVertexBuffers(const std::vector<std::shared_ptr<Buffer>>& buffers) const;
    void bindDescriptorSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, 
        const std::vector<VkDescriptorSet>& descriptorSets) const;
    void setViewport(const VkViewport& viewport) const;
    void setScissor(const VkRect2D& scissor) const;
    void draw(uint32_t vertexCount) const;

private:
    VkCommandBuffer vkCommandBuffer = nullptr;

    DECLARE_VULKAN_FUNCTION(vkBeginCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkEndCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkCmdBeginRenderPass);
    DECLARE_VULKAN_FUNCTION(vkCmdEndRenderPass);
    DECLARE_VULKAN_FUNCTION(vkCmdBindVertexBuffers);
    DECLARE_VULKAN_FUNCTION(vkCmdBindPipeline);
    DECLARE_VULKAN_FUNCTION(vkCmdBindDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkCmdSetViewport);
    DECLARE_VULKAN_FUNCTION(vkCmdSetScissor);
    DECLARE_VULKAN_FUNCTION(vkCmdDraw);
};

} // namespace rfx

