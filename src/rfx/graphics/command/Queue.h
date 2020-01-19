#pragma once

#include "rfx/graphics/command/CommandBuffer.h"

namespace rfx
{

class Queue
{
public:
    explicit Queue(VkQueue vkQueue, const VulkanDeviceFunctionPtrs& vk);

    void submit(const std::shared_ptr<CommandBuffer>& commandBuffer, VkFence fence) const;
    void submit(uint32_t submitCount, const VkSubmitInfo* submits, VkFence fence) const;
    void present(const VkPresentInfoKHR& presentInfo) const;
    void waitIdle() const;

    VkQueue getHandle() const;

private:
    VkQueue vkQueue = nullptr;

    DECLARE_VULKAN_FUNCTION(vkQueueSubmit);
    DECLARE_VULKAN_FUNCTION(vkQueueWaitIdle);
    DECLARE_VULKAN_FUNCTION(vkQueuePresentKHR);
};

}

