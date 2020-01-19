#include "rfx/pch.h"
#include "rfx/graphics/VertexShader.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

VertexShader::VertexShader(const shared_ptr<GraphicsDevice>& graphicsDevice, 
    const VkPipelineShaderStageCreateInfo& stageCreateInfo,
    const VertexFormat& vertexFormat)
        : Shader(graphicsDevice, stageCreateInfo),
          vertexFormat(vertexFormat)
{
    createVertexInputState();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexShader::createVertexInputState()
{
    static const int VERTEX_BUFFER_BIND_ID = 0;

    vertexBindingDescription = {};
    vertexBindingDescription.binding = VERTEX_BUFFER_BIND_ID;
    vertexBindingDescription.stride = vertexFormat.getVertexSize();
    vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    uint32_t location = 0;
    uint32_t offset = 0;

    VkVertexInputAttributeDescription attributeDescription = {};
    attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
    attributeDescription.location = location++;
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.offset = offset;
    vertexAttributeDescriptions.push_back(attributeDescription);
    offset += 12;

    if (vertexFormat.containsColors()) {
        attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
        attributeDescription.location = location++;
        attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescription.offset = offset;
        vertexAttributeDescriptions.push_back(attributeDescription);
        offset += 16;
    }

    if (vertexFormat.containsNormals()) {
        attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
        attributeDescription.location = location++;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = offset;
        vertexAttributeDescriptions.push_back(attributeDescription);
        offset += 12;
    }

    if (vertexFormat.containsTexCoords()) {
        attributeDescription.binding = VERTEX_BUFFER_BIND_ID;
        attributeDescription.location = location++;
        attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescription.offset = offset;
        vertexAttributeDescriptions.push_back(attributeDescription);
        offset += 8;
    }

    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pNext = nullptr;
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = &vertexBindingDescription;
    vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
    vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
}

// ---------------------------------------------------------------------------------------------------------------------

const VertexFormat& VertexShader::getVertexFormat() const
{
    return vertexFormat;
}

// ---------------------------------------------------------------------------------------------------------------------

const VkPipelineVertexInputStateCreateInfo& VertexShader::getVertexInputState() const
{
    return vertexInputState;
}

// ---------------------------------------------------------------------------------------------------------------------
