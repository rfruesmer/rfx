#include "rfx/pch.h"
#include "test/directional-light/VertexDirectionalLightEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

const string VertexDirectionalLightEffect::ID = "vertex_directional_light";

// ---------------------------------------------------------------------------------------------------------------------

VertexDirectionalLightEffect::VertexDirectionalLightEffect(
    const shared_ptr<GraphicsDevice>& graphicsDevice,
    VkRenderPass renderPass,
    std::unique_ptr<ShaderProgram>& shaderProgram)
        : Effect(graphicsDevice, renderPass, shaderProgram) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::createUniformBuffers()
{
    createUniformBuffer(sizeof(UniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::createDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
        createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
    };

    Effect::createDescriptorSetLayout(descriptorSetLayoutBindings);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::createDescriptorPool()
{
    Effect::createDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::updateDescriptorSets()
{
    const vector<VkWriteDescriptorSet> writes = {
        createDescriptorWrite(0, descriptorSets[0], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers[0]->getBufferInfo())
    };

    graphicsDevice->updateDescriptorSets(writes);
}

// ---------------------------------------------------------------------------------------------------------------------

const string& VertexDirectionalLightEffect::getId() const
{
    return ID;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::setModelViewProjMatrix(const mat4& matrix)
{
    uniformData.modelViewProjection = matrix;
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::updateFrom(const vector<shared_ptr<Light>>& lights)
{
    RFX_CHECK_ARGUMENT(!lights.empty());
    RFX_CHECK_ARGUMENT(lights[0]->getType() == LightType::DIRECTIONAL);

    uniformData.lightData = lights[0]->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::updateFrom(const shared_ptr<Material>& material)
{
    uniformData.materialData = material->getData();
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::updateFrom(const shared_ptr<Camera>& camera)
{
    uniformData.modelView = camera->getViewMatrix() * modelMatrix;

    Effect::updateFrom(camera);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexDirectionalLightEffect::updateUniformBuffer()
{
    uniformBuffers[0]->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

