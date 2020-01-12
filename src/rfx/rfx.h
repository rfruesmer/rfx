#pragma once

// #####################################################################################################################
// INCLUDES
// #####################################################################################################################

#ifdef _WINDOWS
#include "res/Resource.h"
#endif // _WINDOWS

#include "rfx/core/Logger.h"

// #####################################################################################################################
// TYPES
// #####################################################################################################################

namespace rfx
{
    
using handle_t = void*;

#define DECLARE_VULKAN_FUNCTION(function) PFN_##function function = nullptr

struct VulkanDeviceFunctionPtrs
{
    DECLARE_VULKAN_FUNCTION(vkGetDeviceProcAddr);
    DECLARE_VULKAN_FUNCTION(vkDeviceWaitIdle);
    DECLARE_VULKAN_FUNCTION(vkDestroyDevice);

    DECLARE_VULKAN_FUNCTION(vkCreateSemaphore);
    DECLARE_VULKAN_FUNCTION(vkDestroySemaphore);

    DECLARE_VULKAN_FUNCTION(vkCreateFence);
    DECLARE_VULKAN_FUNCTION(vkWaitForFences);
    DECLARE_VULKAN_FUNCTION(vkDestroyFence);

    DECLARE_VULKAN_FUNCTION(vkGetDeviceQueue);
    DECLARE_VULKAN_FUNCTION(vkQueueSubmit);
    DECLARE_VULKAN_FUNCTION(vkQueueWaitIdle);
    DECLARE_VULKAN_FUNCTION(vkQueuePresentKHR);

    DECLARE_VULKAN_FUNCTION(vkCreateRenderPass);
    DECLARE_VULKAN_FUNCTION(vkDestroyRenderPass);

    DECLARE_VULKAN_FUNCTION(vkCreateCommandPool);
    DECLARE_VULKAN_FUNCTION(vkDestroyCommandPool);
    DECLARE_VULKAN_FUNCTION(vkAllocateCommandBuffers);
    DECLARE_VULKAN_FUNCTION(vkBeginCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkEndCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkResetCommandBuffer);
    DECLARE_VULKAN_FUNCTION(vkFreeCommandBuffers);
    DECLARE_VULKAN_FUNCTION(vkCmdBeginRenderPass);
    DECLARE_VULKAN_FUNCTION(vkCmdEndRenderPass);
    DECLARE_VULKAN_FUNCTION(vkCmdBindVertexBuffers);
    DECLARE_VULKAN_FUNCTION(vkCmdBindPipeline);
    DECLARE_VULKAN_FUNCTION(vkCmdBindDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkCmdSetViewport);
    DECLARE_VULKAN_FUNCTION(vkCmdSetScissor);
    DECLARE_VULKAN_FUNCTION(vkCmdDraw);
    DECLARE_VULKAN_FUNCTION(vkCmdPipelineBarrier);
    DECLARE_VULKAN_FUNCTION(vkCmdCopyBufferToImage);

    DECLARE_VULKAN_FUNCTION(vkGetImageMemoryRequirements);
    DECLARE_VULKAN_FUNCTION(vkBindImageMemory);
    DECLARE_VULKAN_FUNCTION(vkCreateImage);
    DECLARE_VULKAN_FUNCTION(vkDestroyImage);
    DECLARE_VULKAN_FUNCTION(vkGetImageSubresourceLayout);
    DECLARE_VULKAN_FUNCTION(vkCreateImageView);
    DECLARE_VULKAN_FUNCTION(vkDestroyImageView);

    DECLARE_VULKAN_FUNCTION(vkCreateSampler);
    DECLARE_VULKAN_FUNCTION(vkDestroySampler);

    DECLARE_VULKAN_FUNCTION(vkAllocateMemory);
    DECLARE_VULKAN_FUNCTION(vkFreeMemory);
    DECLARE_VULKAN_FUNCTION(vkCreateBuffer);
    DECLARE_VULKAN_FUNCTION(vkDestroyBuffer);
    DECLARE_VULKAN_FUNCTION(vkGetBufferMemoryRequirements);
    DECLARE_VULKAN_FUNCTION(vkMapMemory);
    DECLARE_VULKAN_FUNCTION(vkUnmapMemory);
    DECLARE_VULKAN_FUNCTION(vkBindBufferMemory);
    DECLARE_VULKAN_FUNCTION(vkInvalidateMappedMemoryRanges);

    DECLARE_VULKAN_FUNCTION(vkCreateDescriptorSetLayout);
    DECLARE_VULKAN_FUNCTION(vkDestroyDescriptorSetLayout);
    DECLARE_VULKAN_FUNCTION(vkCreateDescriptorPool);
    DECLARE_VULKAN_FUNCTION(vkAllocateDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkUpdateDescriptorSets);
    DECLARE_VULKAN_FUNCTION(vkDestroyDescriptorPool);
    DECLARE_VULKAN_FUNCTION(vkCreatePipelineLayout);
    DECLARE_VULKAN_FUNCTION(vkDestroyPipelineLayout);

    DECLARE_VULKAN_FUNCTION(vkCreateShaderModule);
    DECLARE_VULKAN_FUNCTION(vkDestroyShaderModule);

    DECLARE_VULKAN_FUNCTION(vkCreateFramebuffer);
    DECLARE_VULKAN_FUNCTION(vkDestroyFramebuffer);

    DECLARE_VULKAN_FUNCTION(vkCreateGraphicsPipelines);
    DECLARE_VULKAN_FUNCTION(vkDestroyPipeline);

    DECLARE_VULKAN_FUNCTION(vkCreateSwapchainKHR);
    DECLARE_VULKAN_FUNCTION(vkGetSwapchainImagesKHR);
    DECLARE_VULKAN_FUNCTION(vkAcquireNextImageKHR);
    DECLARE_VULKAN_FUNCTION(vkDestroySwapchainKHR);
};

}

// #####################################################################################################################
// CONSTANTS
// #####################################################################################################################

const float RFX_INFINITY = FLT_MAX;
const float RFX_PI = 3.1415926535f;

// #####################################################################################################################
// MACROS
// #####################################################################################################################

#define RFX_THROW(message) {throw std::runtime_error(message + std::string("\n\t") + std::string(__FILE__) + std::string("(") + std::to_string(__LINE__) + std::string(")"));}

#define RFX_CHECK_ARGUMENT(expr, message) {if (!(expr)) {RFX_THROW(std::string("Illegal argument: ") + message);}}
#define RFX_CHECK_STATE(expr, message) {if (!(expr)) {RFX_THROW(std::string("Illegal state: ") + message);}}