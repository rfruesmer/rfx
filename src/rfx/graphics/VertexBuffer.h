#pragma once

#include "rfx/graphics/Buffer.h"
#include "rfx/graphics/VertexFormat.h"

namespace rfx
{

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(
        uint32_t vertexCount, 
        const VertexFormat& vertexFormat,
        VkDevice vkDevice,
        VkBuffer vkBuffer,
        VkDeviceMemory vkDeviceMemory,
        const VulkanDeviceFunctionPtrs& vk,
        size_t size,
        size_t range);

    uint32_t getVertexCount() const;
    VertexFormat getVertexFormat() const;

    const VkPipelineVertexInputStateCreateInfo& getInputState() const;

private:
    void createVertexDescriptions();

    uint32_t vertexCount = 0;
    VertexFormat vertexFormat;

    VkPipelineVertexInputStateCreateInfo inputState = {};
    VkVertexInputBindingDescription bindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
};
    
}
