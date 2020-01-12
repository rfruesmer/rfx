#include "rfx/pch.h"
#include "rfx/graphics/VertexBuffer.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexBuffer::VertexBuffer(
    uint32_t vertexCount,
    const VertexFormat& vertexFormat, 
    VkDevice vkDevice,
    VkBuffer vkBuffer, 
    VkDeviceMemory vkDeviceMemory,
    const VulkanDeviceFunctionPtrs& vk, 
    size_t size, 
    size_t range)
        : Buffer(vkDevice, vkBuffer, vkDeviceMemory, vk, size, range),
          vertexCount(vertexCount),
          vertexFormat(vertexFormat)
{
    createVertexDescriptions();
}

// ---------------------------------------------------------------------------------------------------------------------


void VertexBuffer::createVertexDescriptions()
{
    static const int VERTEX_BUFFER_BIND_ID = 0;

    bindingDescription.binding = VERTEX_BUFFER_BIND_ID;
    bindingDescription.stride = vertexFormat.getVertexSize();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    uint32_t location = 0;
    uint32_t offset = 0;

    VkVertexInputAttributeDescription attributeDescription = {};
    attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
    attributeDescription.location = location++;
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.offset = offset;
    attributeDescriptions.push_back(attributeDescription);
    offset += 12;

    if (vertexFormat.containsColors()) {
        attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
        attributeDescription.location = location++;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = offset;
        attributeDescriptions.push_back(attributeDescription);
        offset += 12;
    }

    if (vertexFormat.containsNormals()) {
        attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
        attributeDescription.location = location++;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = offset;
        attributeDescriptions.push_back(attributeDescription);
        offset += 12;
    }

    if (vertexFormat.containsTexCoords()) {
        attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
        attributeDescription.location = location++;
        attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescription.offset = offset;
        attributeDescriptions.push_back(attributeDescription);
        offset += 8;
    }

    inputState = {};
    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.pNext = nullptr;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescription;
    inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t VertexBuffer::getVertexCount() const
{
    return vertexCount;
}

// ---------------------------------------------------------------------------------------------------------------------

VertexFormat VertexBuffer::getVertexFormat() const
{
    return vertexFormat;
}

// ---------------------------------------------------------------------------------------------------------------------

const VkPipelineVertexInputStateCreateInfo& VertexBuffer::getInputState() const
{
    return inputState;
}

// ---------------------------------------------------------------------------------------------------------------------
