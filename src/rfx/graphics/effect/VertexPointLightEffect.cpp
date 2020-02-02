#include "rfx/pch.h"
#include "rfx/graphics/effect/VertexPointLightEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string VertexPointLightEffect::ID = "vertex_point_light";

// ---------------------------------------------------------------------------------------------------------------------

VertexPointLightEffect::VertexPointLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    std::unique_ptr<ShaderProgram>& shaderProgram)
        : Effect(graphicsDevice, renderPass, shaderProgram)
{
    initUniformBuffer(sizeof(UniformData));
    initDescriptorSetLayout();
    initDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
    initDescriptorSet();
    initPipelineLayout();
    initPipeline();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::initDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    Effect::initDescriptorSetLayout(1, &layoutBinding);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::initDescriptorSet()
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

const string& VertexPointLightEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::setModelViewProjMatrix(const mat4& matrix)
{
    uniformData.modelViewProjection = matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::updateFrom(const vector<shared_ptr<Light>>& lights)
{
    RFX_CHECK_ARGUMENT(!lights.empty());
    RFX_CHECK_ARGUMENT(lights[0]->getType() == LightType::POINT);

    uniformData.lightData = lights[0]->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::updateFrom(const shared_ptr<Material>& material)
{
    uniformData.materialData = material->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::updateFrom(const shared_ptr<Camera>& camera)
{
    uniformData.modelView = camera->getViewMatrix() * modelMatrix;

    Effect::updateFrom(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::updateUniformBuffer()
{
    uniformBuffer->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

