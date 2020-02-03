#include "rfx/pch.h"
#include "test/texture-mapping/Texture2DEffect.h"


using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

const string Texture2DEffect::ID = "texture2D";

// ---------------------------------------------------------------------------------------------------------------------

Texture2DEffect::Texture2DEffect(const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    unique_ptr<ShaderProgram>& shaderProgram,
    const shared_ptr<Texture2D>& texture)
        : Effect(graphicsDevice, renderPass, shaderProgram),
          texture(texture)
{
    RFX_CHECK_ARGUMENT(texture != nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DEffect::createUniformBuffers()
{
    createUniformBuffer(sizeof(mat4));
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DEffect::createDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
        createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
        createDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
    };

    Effect::createDescriptorSetLayout(descriptorSetLayoutBindings);
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DEffect::createDescriptorPool()
{
    Effect::createDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DEffect::updateDescriptorSets()
{
    const vector<VkWriteDescriptorSet> writes = {
        createDescriptorWrite(0, descriptorSets[0], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers[0]->getBufferInfo()),
        createDescriptorWrite(1, descriptorSets[0], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture->getDescriptorImageInfo())
    };

    graphicsDevice->updateDescriptorSets(writes);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Texture2DEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DEffect::setModelViewProjMatrix(const mat4& matrix)
{
    uniformData.modelViewProjection = matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void Texture2DEffect::updateUniformBuffer()
{
    uniformBuffers[0]->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------
