#pragma once

namespace rfx
{

class Queue
{
public:
    explicit Queue(VkQueue vkQueue, const VulkanDeviceFunctionPtrs& vk);

    void submit(uint32_t submitCount, const VkSubmitInfo* submits, VkFence fence) const;
    void present(const VkPresentInfoKHR& presentInfo) const;

private:
    VkQueue vkQueue = nullptr;

    DECLARE_VULKAN_FUNCTION(vkQueueSubmit);
    DECLARE_VULKAN_FUNCTION(vkQueuePresentKHR);
};

}

