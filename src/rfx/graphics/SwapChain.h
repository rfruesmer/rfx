#pragma once

#include "SwapChainDesc.h"


namespace rfx {

class SwapChain
{
public:
    SwapChain(VkDevice device, VkSwapchainKHR swapChain, SwapChainDesc swapChainDesc);
    ~SwapChain();

    void createFrameBuffers(VkRenderPass renderPass);

    [[nodiscard]] VkSwapchainKHR getHandle() const;
    [[nodiscard]] const SwapChainDesc& getDesc() const;
    [[nodiscard]] const std::vector<VkFramebuffer>& getFramebuffers() const;

private:
    VkDevice device = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    SwapChainDesc desc;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;
};

}
