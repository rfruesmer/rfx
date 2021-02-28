#pragma once


#include "rfx/graphics/GraphicsContext.h"
#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/application/Window.h"

namespace rfx {

class DevTools
{
public:
    DevTools(
        const std::shared_ptr<Window>& window,
        const std::unique_ptr<GraphicsContext>& graphicsContext,
        std::shared_ptr<GraphicsDevice> graphicsDevice);

    ~DevTools();

    void beginDraw(uint32_t frameIndex, uint32_t lastFPS);
    void endDraw();

    bool sliderFloat(const char* caption, float* value, float min, float max);
    bool checkBox(const char* caption, bool* value);

    [[nodiscard]] VkCommandBuffer getCommandBuffer(uint32_t frameIndex) const;

private:
    static void checkResult(VkResult result);

    void createCommandPool(VkCommandPool* commandPool, VkCommandPoolCreateFlags createFlags);
    void createCommandBuffers(
        uint32_t count,
        VkCommandPool commandPool,
        VkCommandBuffer* outCommandBuffers);

    std::shared_ptr<GraphicsDevice> graphicsDevice_;
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers_;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> frameBuffers;
    uint32_t currentFrameIndex;
};

} // namespace rfx