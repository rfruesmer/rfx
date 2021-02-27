#include "rfx/pch.h"
#include "rfx/graphics/Queue.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Queue::Queue(VkQueue queue, uint32_t familyIndex, VkDevice device)
    : queue(queue),
      familyIndex(familyIndex),
      device(device) {}

// ---------------------------------------------------------------------------------------------------------------------

VkQueue Queue::getHandle() const
{
    return queue;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Queue::getFamilyIndex() const
{
    return familyIndex;
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::submit(const shared_ptr<CommandBuffer>& commandBuffer) const
{
    submit(commandBuffer, VK_NULL_HANDLE);
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::submit(const shared_ptr<CommandBuffer>& commandBuffer, VkFence fence) const
{
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer->getHandle()
    };

    submit(submitInfo, fence);
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::submit(const VkSubmitInfo& submitInfo, VkFence fence) const
{
    ThrowIfFailed(vkQueueSubmit(
        queue,
        1,
        &submitInfo,
        fence));
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::flush(const shared_ptr<CommandBuffer>& commandBuffer) const
{
    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
    };

    ThrowIfFailed(vkCreateFence(
        device,
        &fenceCreateInfo,
        nullptr,
        &fence));

    submit(commandBuffer, fence);

    ThrowIfFailed(vkWaitForFences(
        device,
        1,
        &fence,
        VK_TRUE,
        DEFAULT_FENCE_TIMEOUT));

    vkDestroyFence(device, fence, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

VkResult Queue::present(const VkPresentInfoKHR& presentInfo) const
{
    return vkQueuePresentKHR(queue, &presentInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void Queue::waitIdle() const
{
    vkQueueWaitIdle(queue);
}

// ---------------------------------------------------------------------------------------------------------------------
