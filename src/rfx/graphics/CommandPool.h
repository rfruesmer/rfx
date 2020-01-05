#pragma once

#include "rfx/graphics/CommandBuffer.h"

namespace rfx
{
    
class CommandPool
{
    friend class GraphicsDevice;

public:
    explicit CommandPool(VkDevice vkDevice, 
        VkCommandPool vkCommandPool,
        const VulkanDeviceFunctionPtrs& vk);
    ~CommandPool();

    VkCommandPool getHandle() const;

    bool isValid() const;

    void clear();

    std::shared_ptr<CommandBuffer> createCommandBuffer();
    void freeCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer);
    void freeCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& commandBuffers);

private:
    void invalidate();

    VkDevice vkDevice = nullptr;
    VkCommandPool vkCommandPool = nullptr;
    VulkanDeviceFunctionPtrs vk = {};

    std::unordered_set<std::shared_ptr<CommandBuffer>> commandBuffers;

    DECLARE_VULKAN_FUNCTION(vkAllocateCommandBuffers);
    DECLARE_VULKAN_FUNCTION(vkFreeCommandBuffers);
    DECLARE_VULKAN_FUNCTION(vkResetCommandBuffer);
};

}
