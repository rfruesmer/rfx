#include "rfx/pch.h"
#include "TestMaterialShader.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TestMaterialShader::TestMaterialShader(
    shared_ptr<GraphicsDevice> graphicsDevice,
    shared_ptr<Model> scene)
    : MaterialShader(move(graphicsDevice)),
      scene_(move(scene)) {}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::createUniformBuffers()
{
    createMeshDataBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::createMeshDataBuffers()
{
    for (const auto& node : scene_->getGeometryNodes()) {
        shared_ptr<Buffer> meshDataBuffer = graphicsDevice_->createBuffer(
            sizeof(MeshData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice_->bind(meshDataBuffer);
        meshDataBuffer->load(sizeof(mat4), &node->getWorldTransform());
        meshDataBuffers_.push_back(meshDataBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::createDescriptorSetLayouts()
{
    createMeshDescriptorSetLayout();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::createMeshDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    const VkDescriptorSetLayoutCreateInfo meshDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &meshDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &meshDescSetLayoutCreateInfo,
        nullptr,
        &descriptorSetLayouts_[DescriptorType::MESH]));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::createDescriptorSets(VkDescriptorPool descriptorPool)
{
    createMeshDescriptorSets(descriptorPool);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::createMeshDescriptorSets(VkDescriptorPool descriptorPool)
{
    meshDescriptorSets_.resize(scene_->getGeometryNodeCount());

    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayouts_[DescriptorType::MESH]
    };

    for (uint32_t i = 0, count = scene_->getGeometryNodeCount(); i < count; ++i) {

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &meshDescriptorSets_[i]));

        const VkWriteDescriptorSet writeDescriptorSet =
            buildWriteDescriptorSet(
                meshDescriptorSets_[i],
                0,
                &meshDataBuffers_[i]->getDescriptorBufferInfo());

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

VkWriteDescriptorSet TestMaterialShader::buildWriteDescriptorSet(
    VkDescriptorSet descriptorSet,
    uint32_t binding,
    const VkDescriptorBufferInfo* descriptorBufferInfo)
{
    return {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = descriptorBufferInfo
    };
}

// ---------------------------------------------------------------------------------------------------------------------

vector<VkDescriptorSetLayout> TestMaterialShader::getDescriptorSetLayouts() const
{
    return descriptorSetLayouts_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestMaterialShader::getMeshDescriptorSets() const
{
    return meshDescriptorSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestMaterialShader::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    for (int i = 0; i < DescriptorType::QUANTITY; ++i) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayouts_[i], nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

