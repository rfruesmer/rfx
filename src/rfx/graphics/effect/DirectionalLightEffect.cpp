#include "rfx/pch.h"
#include "rfx/graphics/effect/DirectionalLightEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

const string DirectionalLightEffect::ID = "directional_vertex_lighting";

// ---------------------------------------------------------------------------------------------------------------------

DirectionalLightEffect::DirectionalLightEffect(
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

void DirectionalLightEffect::initDescriptorSetLayout()
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

void DirectionalLightEffect::initDescriptorSet()
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

const string& DirectionalLightEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void DirectionalLightEffect::setModelViewProjMatrix(const mat4& matrix)
{
    uniformData.modelViewProjection = matrix;

    uniformBuffer->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

void DirectionalLightEffect::setLights(const vector<shared_ptr<Light>>& lights)
{
    RFX_CHECK_ARGUMENT(!lights.empty());
    RFX_CHECK_ARGUMENT(lights[0]->getType() == LightType::DIRECTIONAL);

    uniformData.lightData = lights[0]->getData();

    uniformBuffer->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

void DirectionalLightEffect::setMaterial(const shared_ptr<Material>& material)
{
    uniformData.materialData = material->getData();

    uniformBuffer->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

