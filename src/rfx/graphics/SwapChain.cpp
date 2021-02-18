#include "rfx/pch.h"
#include "SwapChain.h"

#include <utility>

using namespace rfx;

// ---------------------------------------------------------------------------------------------------------------------

SwapChain::SwapChain(VkDevice device, VkSwapchainKHR swapChain, SwapChainDesc swapChainDesc)
    : device(device), swapChain(swapChain), desc(std::move(swapChainDesc))
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

void SwapChain::createFrameBuffers(VkRenderPass renderPass)
{
    framebuffers.resize(desc.bufferCount);

    VkFramebufferCreateInfo framebufferInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = 1,
        .width = desc.extent.width,
        .height = desc.extent.height,
        .layers = 1
    };

    for (size_t i = 0; i < desc.bufferCount; ++i) {
        VkImageView attachments[] = {
            imageViews[i]
        };

        framebufferInfo.pAttachments = attachments;

        ThrowIfFailed(vkCreateFramebuffer(
            device,
            &framebufferInfo,
            nullptr,
            &framebuffers[i]));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<VkFramebuffer>& SwapChain::getFramebuffers() const
{
    return framebuffers;
}

// ---------------------------------------------------------------------------------------------------------------------
