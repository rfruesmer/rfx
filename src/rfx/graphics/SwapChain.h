#pragma once

#include "rfx/graphics/SwapChainDesc.h"
#include "rfx/graphics/DepthBuffer.h"


namespace rfx {

class SwapChain
{
public:
    SwapChain(
        VkDevice device,
        VkSwapchainKHR swapChain,
        SwapChainDesc swapChainDesc);
    ~SwapChain();

    void createFrameBuffers(
        VkRenderPass renderPass,
        const std::unique_ptr<DepthBuffer>& depthBuffer,
        VkImageView multiSampleImageView);

    [[nodiscard]] VkSwapchainKHR getHandle() const;
    [[nodiscard]] const SwapChainDesc& getDesc() const;
    [[nodiscard]] const std::vector<VkFramebuffer>& getFramebuffers() const;
    [[nodiscard]] const std::vector<VkImageView>& getImageViews() const;

private:
    VkDevice device = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    SwapChainDesc desc;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;
};

}
