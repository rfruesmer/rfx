#pragma once

#include "rfx/graphics/CommandBuffer.h"

namespace rfx {

class Queue
{
public:
    explicit Queue(VkQueue queue, uint32_t familyIndex);

    void submit(const std::shared_ptr<CommandBuffer>& commandBuffer) const;
    void submit(const std::shared_ptr<CommandBuffer>& commandBuffer, VkFence fence) const;
    void submit(const VkSubmitInfo& submitInfo, VkFence fence) const;
    [[nodiscard]] VkResult present(const VkPresentInfoKHR& presentInfo) const;
    void waitIdle() const;

    [[nodiscard]] VkQueue getHandle() const;
    [[nodiscard]] uint32_t getFamilyIndex() const;

private:
    VkQueue queue = VK_NULL_HANDLE;
    uint32_t familyIndex = 0;
};

} // namespace rfx
