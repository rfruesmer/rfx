#include "rfx/pch.h"
#include "test/directional-light/FragmentDirectionalLightEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string FragmentDirectionalLightEffect::ID = "fragment_directional_light";

// ---------------------------------------------------------------------------------------------------------------------

FragmentDirectionalLightEffect::FragmentDirectionalLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    std::unique_ptr<ShaderProgram>& shaderProgram)
        : Effect(graphicsDevice, renderPass, shaderProgram)
{
    initUniformBuffer(sizeof(VertexShaderConstants));
    initUniformBuffer(sizeof(FragmentShaderConstants));
    initDescriptorSetLayout();
    initDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2}
    });
    initDescriptorSet();
    initPipelineLayout();
    initPipeline();
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::initDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding layoutBindings[2] = {};
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[0].pImmutableSamplers = nullptr;

    layoutBindings[1].binding = 0;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layoutBindings[1].pImmutableSamplers = nullptr;

    Effect::initDescriptorSetLayout(1, layoutBindings);
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::initDescriptorSet()
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    graphicsDevice->allocateDescriptorSets(descriptorSetAllocateInfo, descriptorSets);

    VkWriteDescriptorSet writes[2] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptorSets[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &uniformBuffers[0]->getBufferInfo();
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].pNext = nullptr;
    writes[1].dstSet = descriptorSets[0];
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[1].pBufferInfo = &uniformBuffers[1]->getBufferInfo();
    writes[1].dstArrayElement = 0;
    writes[1].dstBinding = 0;

    graphicsDevice->updateDescriptorSets(2, writes);
}

// ---------------------------------------------------------------------------------------------------------------------

const string& FragmentDirectionalLightEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::setModelViewProjMatrix(const mat4& matrix)
{
    vertexShaderConstants.modelViewProjection = matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::updateFrom(const vector<shared_ptr<Light>>& lights)
{
    RFX_CHECK_ARGUMENT(!lights.empty());
    RFX_CHECK_ARGUMENT(lights[0]->getType() == LightType::DIRECTIONAL);

    fragmentShaderConstants.lightData = lights[0]->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::updateFrom(const shared_ptr<Material>& material)
{
    fragmentShaderConstants.materialData = material->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::updateFrom(const shared_ptr<Camera>& camera)
{
    vertexShaderConstants.modelView = camera->getViewMatrix() * modelMatrix;
    fragmentShaderConstants.modelView = vertexShaderConstants.modelView;

    Effect::updateFrom(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::updateUniformBuffer()
{
    uniformBuffers[0]->load(sizeof(VertexShaderConstants),
        reinterpret_cast<std::byte*>(&vertexShaderConstants));

    uniformBuffers[1]->load(sizeof(FragmentShaderConstants),
        reinterpret_cast<std::byte*>(&fragmentShaderConstants));
}

// ---------------------------------------------------------------------------------------------------------------------

