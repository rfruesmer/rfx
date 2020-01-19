#include "rfx/pch.h"
#include "rfx/graphics/Texture2DEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

Texture2DEffect::Texture2DEffect(const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const shared_ptr<Texture2D>& texture)
        : Effect(graphicsDevice, renderPass, shaderProgram)
{
    uniformBuffer = graphicsDevice->createUniformBuffer(sizeof(mat4));
    uniformBuffer->bind();

    VkWriteDescriptorSet writes[2] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptorSets[0];
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniformBuffer->getBufferInfo();

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].pNext = nullptr;
    writes[1].dstSet = descriptorSets[0];
    writes[1].dstBinding = 1;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = &texture->getDescriptorImageInfo();

    graphicsDevice->updateDescriptorSets(2, writes);
}

// ---------------------------------------------------------------------------------------------------------------------
