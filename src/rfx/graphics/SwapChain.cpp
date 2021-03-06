#include "rfx/pch.h"
#include "SwapChain.h"

#include <utility>

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

SwapChain::SwapChain(VkDevice device, VkSwapchainKHR swapChain, SwapChainDesc swapChainDesc)
    : device(device),
      swapChain(swapChain),
      desc(move(swapChainDesc))
{
    ThrowIfFailed(vkGetSwapchainImagesKHR(
        device,
        swapChain,
        &desc.bufferCount,
        nullptr));

    images.resize(desc.bufferCount);
    ThrowIfFailed(vkGetSwapchainImagesKHR(
        device,
        swapChain,
        &desc.bufferCount,
        images.data()));

    VkImageViewCreateInfo imageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = desc.format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    imageViews.resize(desc.bufferCount);

    for (size_t i = 0; i < desc.bufferCount; ++i) {
        imageViewCreateInfo.image = images[i];
        ThrowIfFailed(vkCreateImageView(
            device,
            &imageViewCreateInfo,
            nullptr,
            &imageViews[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SwapChain::~SwapChain()
{
    for (const auto& framebuffer : framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (const auto& imageView : imageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

VkSwapchainKHR SwapChain::getHandle() const
{
    return swapChain;
}

// ---------------------------------------------------------------------------------------------------------------------

const SwapChainDesc& SwapChain::getDesc() const
{
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------

void SwapChain::createFrameBuffers(
    VkRenderPass renderPass,
    const unique_ptr<DepthBuffer>& depthBuffer,
    VkImageView multiSampleImageView)
{
    framebuffers.resize(desc.bufferCount);

    VkFramebufferCreateInfo framebufferInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .width = desc.extent.width,
        .height = desc.extent.height,
        .layers = 1
    };

    for (size_t i = 0; i < desc.bufferCount; ++i)
    {
        vector<VkImageView> attachments;
        if (multiSampleImageView != VK_NULL_HANDLE) {
            attachments.push_back(multiSampleImageView);
        }
        attachments.push_back(imageViews[i]);
        if (depthBuffer != nullptr) {
            attachments.push_back(depthBuffer->getImageView());
        }

        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();

        ThrowIfFailed(vkCreateFramebuffer(
            device,
            &framebufferInfo,
            nullptr,
            &framebuffers[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkFramebuffer>& SwapChain::getFramebuffers() const
{
    return framebuffers;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkImageView>& SwapChain::getImageViews() const
{
    return imageViews;
}

// ---------------------------------------------------------------------------------------------------------------------
