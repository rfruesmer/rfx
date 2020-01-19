#include "rfx/pch.h"

#include "rfx/graphics/command/Queue.h"

using namespace rfx;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

Queue::Queue(VkQueue vkQueue,
    const VulkanDeviceFunctionPtrs& vk)
        : vkQueue(vkQueue)
{
    vkQueueSubmit = vk.vkQueueSubmit;
    vkQueueWaitIdle = vk.vkQueueWaitIdle;
    vkQueuePresentKHR = vk.vkQueuePresentKHR;
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::submit(const shared_ptr<CommandBuffer>& commandBuffer, VkFence fence) const
{
    const VkCommandBuffer vkCommandBuffer = commandBuffer->getHandle();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    submit(1, &submitInfo, fence);
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::submit(uint32_t submitCount, const VkSubmitInfo* submits, VkFence fence) const
{
    const VkResult result = vkQueueSubmit(vkQueue, submitCount, submits, fence);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to submit to queue");
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::present(const VkPresentInfoKHR& presentInfo) const
{
    const VkResult result = vkQueuePresentKHR(vkQueue, &presentInfo);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to present queue");
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::waitIdle() const
{
    const VkResult result = vkQueueWaitIdle(vkQueue);
    RFX_CHECK_STATE(result == VK_SUCCESS, "Failed to wait until idle");
}

// ---------------------------------------------------------------------------------------------------------------------

VkQueue Queue::getHandle() const
{
    return vkQueue;
}

// ---------------------------------------------------------------------------------------------------------------------