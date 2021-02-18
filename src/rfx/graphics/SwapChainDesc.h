#pragma once

#include "rfx/graphics/SurfaceDesc.h"


namespace rfx {

struct SwapChainDesc {
    SurfaceDesc surface;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D extent {};
    uint32_t bufferCount = 0;
};

} // namespace rfx