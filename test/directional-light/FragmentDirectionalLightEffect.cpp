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
        : Effect(graphicsDevice, renderPass, shaderProgram) {}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::createUniformBuffers()
{
    createUniformBuffer(sizeof(VertexShaderConstants));
    createUniformBuffer(sizeof(FragmentShaderConstants));
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::createDescriptorSetLayout()
{
    const vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
        createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
        createDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
    };

    Effect::createDescriptorSetLayout(descriptorSetLayoutBindings);
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::createDescriptorPool()
{
    Effect::createDescriptorPool({
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2}
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void FragmentDirectionalLightEffect::updateDescriptorSets()
{
    const vector<VkWriteDescriptorSet> writes = {
        createDescriptorWrite(0, descriptorSets[0], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers[0]->getBufferInfo()),
        createDescriptorWrite(1, descriptorSets[0], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers[1]->getBufferInfo())
    };

    graphicsDevice->updateDescriptorSets(writes);
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

