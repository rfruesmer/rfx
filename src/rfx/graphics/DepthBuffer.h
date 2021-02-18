#pragma once

namespace rfx {

class DepthBuffer
{
public:

private:
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
};

} // namespace rfx
