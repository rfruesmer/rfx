#include "rfx/pch.h"
#include "rfx/graphics/VertexColorEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

VertexColorEffect::VertexColorEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    std::unique_ptr<ShaderProgram>& shaderProgram)
        : Effect(graphicsDevice, renderPass, shaderProgram)
{
    initUniformBuffer();
    initDescriptorSetLayout();
    initDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
    initDescriptorSet();
    initPipelineLayout();
    initPipeline();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::initDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &layoutBinding;

    descriptorSetLayout = graphicsDevice->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::initDescriptorSet()
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    graphicsDevice->allocateDescriptorSets(descriptorSetAllocateInfo, descriptorSets);

    VkWriteDescriptorSet writes = {};
    writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes.pNext = nullptr;
    writes.dstSet = descriptorSets[0];
    writes.descriptorCount = 1;
    writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes.pBufferInfo = &uniformBuffer->getBufferInfo();
    writes.dstArrayElement = 0;
    writes.dstBinding = 0;

    graphicsDevice->updateDescriptorSets(1, &writes);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::initUniformBuffer()
{
    uniformBuffer = graphicsDevice->createUniformBuffer(sizeof(mat4));
    uniformBuffer->bind();
}

// ---------------------------------------------------------------------------------------------------------------------
