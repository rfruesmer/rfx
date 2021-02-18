#include "rfx/pch.h"
#include "rfx/graphics/VertexShader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexShader::VertexShader(
    VkDevice vkDevice,
    const VkPipelineShaderStageCreateInfo& createInfo,
    const VertexFormat& vertexFormat)
        : Shader(vkDevice, createInfo),
          vertexFormat(vertexFormat)
{
    createVertexInputState();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexShader::createVertexInputState()
{
    static const int VERTEX_BUFFER_BIND_ID = 0;

    vertexBindingDescription = {
        .binding = VERTEX_BUFFER_BIND_ID,
        .stride = vertexFormat.getVertexSize(),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    uint32_t location = 0;
    uint32_t offset = 0;

    VkVertexInputAttributeDescription attributeDescription = {
        .location = location++,
        .binding = VERTEX_BUFFER_BIND_ID,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offset
    };
    vertexAttributeDescriptions.push_back(attributeDescription);
    offset += 12;

    if (vertexFormat.containsColors()) {
        attributeDescription = {
            .location = location++,
            .binding = VERTEX_BUFFER_BIND_ID,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offset
        };
        vertexAttributeDescriptions.push_back(attributeDescription);
        offset += 16;
    }

    if (vertexFormat.containsNormals()) {
        attributeDescription = {
            .location = location++,
            .binding = VERTEX_BUFFER_BIND_ID,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offset
        };
        vertexAttributeDescriptions.push_back(attributeDescription);
        offset += 12;
    }

    if (vertexFormat.containsTexCoords()) {
        attributeDescription = {
            .location = location++,
            .binding = VERTEX_BUFFER_BIND_ID,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offset
        };
        vertexAttributeDescriptions.push_back(attributeDescription);
        offset += 8;
    }

    vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size()),
        .pVertexAttributeDescriptions = vertexAttributeDescriptions.data()
    };
}

// ---------------------------------------------------------------------------------------------------------------------

const VertexFormat& VertexShader::getVertexFormat() const
{
    return vertexFormat;
}

// ---------------------------------------------------------------------------------------------------------------------

const VkPipelineVertexInputStateCreateInfo& VertexShader::getVertexInputStateCreateInfo() const
{
    return vertexInputStateCreateInfo;
}

// ---------------------------------------------------------------------------------------------------------------------
