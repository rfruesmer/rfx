#include "rfx/pch.h"
#include "TestEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TestEffect::TestEffect(
    shared_ptr<GraphicsDevice> graphicsDevice,
    shared_ptr<Scene> scene)
    : graphicsDevice_(move(graphicsDevice)),
      scene_(move(scene)) {}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createUniformBuffers()
{
    createSceneDataBuffer();
    createMeshDataBuffers();
    createMaterialDataBuffers();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDataBuffer()
{
    sceneDataBuffer_ = graphicsDevice_->createBuffer(
        getSceneDataSize(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    graphicsDevice_->bind(sceneDataBuffer_);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDataBuffers()
{
    for (const auto& mesh : scene_->getMeshes()) {
        shared_ptr<Buffer> meshDataBuffer = graphicsDevice_->createBuffer(
            sizeof(MeshData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        graphicsDevice_->bind(meshDataBuffer);
        meshDataBuffer->load(sizeof(mat4), &mesh->getWorldTransform());
        meshDataBuffers_.push_back(meshDataBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMaterialDataBuffers()
{
    MaterialData materialData {
        .shininess = 100.0f
    };

    for (const auto& material : scene_->getMaterials()) {
        shared_ptr<Buffer> materialUniformBuffer = graphicsDevice_->createBuffer(
            sizeof(MaterialData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        materialData.baseColor = material->getBaseColorFactor();

        graphicsDevice_->bind(materialUniformBuffer);
        materialUniformBuffer->load(sizeof(MaterialData), &materialData);
        materialDataBuffers_.push_back(materialUniformBuffer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorPool()
{
    const uint32_t meshCount = scene_->getMeshCount();
    const uint32_t materialCount = scene_->getMaterialCount();

    vector<VkDescriptorPoolSize> poolSizes {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1 + materialCount + meshCount
        }
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1 + materialCount + meshCount,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice_->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorSetLayouts()
{
    createSceneDescriptorSetLayout();
    createMaterialDescriptorSetLayout();
    createMeshDescriptorSetLayout();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    VkDescriptorSetLayoutCreateInfo sceneDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &sceneDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &sceneDescSetLayoutCreateInfo,
        nullptr,
        &sceneDescSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMaterialDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding materialDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    VkDescriptorSetLayoutCreateInfo materialDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &materialDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &materialDescSetLayoutCreateInfo,
        nullptr,
        &materialDescSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding meshDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    VkDescriptorSetLayoutCreateInfo meshDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &meshDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &meshDescSetLayoutCreateInfo,
        nullptr,
        &meshDescSetLayout_));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorSets()
{
    createSceneDescriptorSet();
    createMaterialDescriptorSets();
    createMeshDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &sceneDescSetLayout_
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        &allocInfo,
        &sceneDescSet_));

    VkDescriptorBufferInfo bufferInfo {
        .buffer = sceneDataBuffer_->getHandle(),
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    VkWriteDescriptorSet writeDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = sceneDescSet_,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };

    vkUpdateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMaterialDescriptorSets()
{
    materialDescSets_.resize(scene_->getMaterialCount());

    for (uint32_t i = 0, count = scene_->getMaterialCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &materialDescSetLayout_
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &materialDescSets_[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = materialDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = materialDescSets_[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSets()
{
    meshDescSets_.resize(scene_->getMeshCount());

    for (uint32_t i = 0, count = scene_->getMeshCount(); i < count; ++i) {

        VkDescriptorSetAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &meshDescSetLayout_
        };

        ThrowIfFailed(vkAllocateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            &allocInfo,
            &meshDescSets_[i]));

        VkDescriptorBufferInfo bufferInfo {
            .buffer = meshDataBuffers_[i]->getHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        VkWriteDescriptorSet writeDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = meshDescSets_[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo
        };

        vkUpdateDescriptorSets(
            graphicsDevice_->getLogicalDevice(),
            1,
            &writeDescriptorSet,
            0,
            nullptr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

vector<VkDescriptorSetLayout> TestEffect::getDescriptorSetLayouts()
{
    return {
        sceneDescSetLayout_,
        materialDescSetLayout_,
        meshDescSetLayout_
    };
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet TestEffect::getSceneDescriptorSet() const
{
    return sceneDescSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMaterialDescriptorSets() const
{
    return materialDescSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMeshDescriptorSets() const
{
    return meshDescSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    vkDestroyDescriptorSetLayout(device, sceneDescSetLayout_, nullptr);
    vkDestroyDescriptorSetLayout(device, meshDescSetLayout_, nullptr);
    vkDestroyDescriptorSetLayout(device, materialDescSetLayout_, nullptr);
    vkDestroyDescriptorPool(device, descriptorPool_, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

