#include "rfx/pch.h"
#include "test/basic/VertexColorEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string VertexColorEffect::ID = "vertex_color";

// ---------------------------------------------------------------------------------------------------------------------

VertexColorEffect::VertexColorEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    std::unique_ptr<ShaderProgram>& shaderProgram)
        : Effect(graphicsDevice, renderPass, shaderProgram) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::createUniformBuffers()
{
    createUniformBuffer(sizeof(mat4));
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::createDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
        createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
    };

    Effect::createDescriptorSetLayout(descriptorSetLayoutBindings);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::createDescriptorPool()
{
    Effect::createDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::updateDescriptorSets()
{
    const vector<VkWriteDescriptorSet> writes = {
        createDescriptorWrite(0, descriptorSets[0], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers[0]->getBufferInfo())
    };

    graphicsDevice->updateDescriptorSets(writes);
}

// ---------------------------------------------------------------------------------------------------------------------

const string& VertexColorEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::setModelViewProjMatrix(const mat4& matrix)
{
    uniformData.modelViewProjection = matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexColorEffect::updateUniformBuffer()
{
    uniformBuffers[0]->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------
