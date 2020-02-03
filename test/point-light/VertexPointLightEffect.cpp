#include "rfx/pch.h"
#include "test/point-light/VertexPointLightEffect.h"

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
        : Effect(graphicsDevice, renderPass, shaderProgram) {}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::createUniformBuffers()
{
    createUniformBuffer(sizeof(UniformData));
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::createDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
         createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
    };

    Effect::createDescriptorSetLayout(descriptorSetLayoutBindings);
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::createDescriptorPool()
{
    Effect::createDescriptorPool({
         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void VertexPointLightEffect::updateDescriptorSets()
{
    const vector<VkWriteDescriptorSet> writes = {
        createDescriptorWrite(0, descriptorSets[0], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers[0]->getBufferInfo())
    };

    graphicsDevice->updateDescriptorSets(writes);
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
    uniformBuffers[0]->load(sizeof(UniformData),
        reinterpret_cast<std::byte*>(&uniformData));
}


// ---------------------------------------------------------------------------------------------------------------------

