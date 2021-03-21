#include "rfx/pch.h"
#include "TestEffect.h"

using namespace rfx;
using namespace glm;
using namespace std;


// ---------------------------------------------------------------------------------------------------------------------

TestEffect::TestEffect(
    shared_ptr<GraphicsDevice> graphicsDevice,
    shared_ptr<Model> scene)
    : Effect(move(graphicsDevice)),
      scene_(move(scene)) {}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createUniformBuffers()
{
    createSceneDataBuffer();
    createMeshDataBuffers();
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

void TestEffect::createDescriptorPools()
{
    const uint32_t meshCount = scene_->getGeometryNodeCount();
    const uint32_t materialCount = scene_->getMaterialCount();
    const uint32_t uniformBufferCount = 1 + meshCount + materialCount;

    uint32_t imageSamplerCount = getVertexFormat().containsTexCoords() ? 1 : 0;
    imageSamplerCount += getVertexFormat().containsTangents() ? 1 : 0;
    imageSamplerCount += scene_->getMaterial(0)->getMetallicRoughnessTexture() != nullptr ? 1 : 0; // TODO: extract imageSamplerCount to argument
    imageSamplerCount += scene_->getMaterial(0)->getOcclusionTexture() != nullptr ? 1 : 0; // TODO: extract imageSamplerCount to argument
    imageSamplerCount += scene_->getMaterial(0)->getEmissiveTexture() != nullptr ? 1 : 0; // TODO: extract imageSamplerCount to argument

    vector<VkDescriptorPoolSize> descriptorPoolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBufferCount}
    };

    if (imageSamplerCount > 0) {
        descriptorPoolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageSamplerCount });
    }

    descriptorPool_ = createDescriptorPool(descriptorPoolSizes, uniformBufferCount + imageSamplerCount);
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorPool TestEffect::createDescriptorPool(
    const vector<VkDescriptorPoolSize>& poolSizes,
    uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo poolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    VkDescriptorPool descriptorPool;
    ThrowIfFailed(vkCreateDescriptorPool(
        graphicsDevice_->getLogicalDevice(),
        &poolCreateInfo,
        nullptr,
        &descriptorPool));

    return descriptorPool;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createDescriptorSetLayouts()
{
    createSceneDescriptorSetLayout();
    createMeshDescriptorSetLayout();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding sceneDescSetLayoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    const VkDescriptorSetLayoutCreateInfo sceneDescSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &sceneDescSetLayoutBinding
    };

    ThrowIfFailed(vkCreateDescriptorSetLayout(
        graphicsDevice_->getLogicalDevice(),
        &sceneDescSetLayoutCreateInfo,
        nullptr,
        &descriptorSetLayouts_[DescriptorType::SCENE]));
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSetLayout()
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

void TestEffect::createDescriptorSets()
{
    createSceneDescriptorSet();
    createMeshDescriptorSets();
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createSceneDescriptorSet()
{
    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayouts_[DescriptorType::SCENE]
    };

    ThrowIfFailed(vkAllocateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        &allocInfo,
        &sceneDescriptorSet_));

    const VkWriteDescriptorSet writeDescriptorSet =
        buildWriteDescriptorSet(sceneDescriptorSet_, 0, &sceneDataBuffer_->getDescriptorBufferInfo());

    vkUpdateDescriptorSets(
        graphicsDevice_->getLogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::createMeshDescriptorSets()
{
    meshDescriptorSets_.resize(scene_->getGeometryNodeCount());

    const VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool_,
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

VkWriteDescriptorSet TestEffect::buildWriteDescriptorSet(
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

VkDescriptorPool TestEffect::getDescriptorPool() const
{
    return descriptorPool_;
}

// ---------------------------------------------------------------------------------------------------------------------

vector<VkDescriptorSetLayout> TestEffect::getDescriptorSetLayouts() const
{
    return descriptorSetLayouts_;
}

// ---------------------------------------------------------------------------------------------------------------------

VkDescriptorSet TestEffect::getSceneDescriptorSet() const
{
    return sceneDescriptorSet_;
}

// ---------------------------------------------------------------------------------------------------------------------

const vector<VkDescriptorSet>& TestEffect::getMeshDescriptorSets() const
{
    return meshDescriptorSets_;
}

// ---------------------------------------------------------------------------------------------------------------------

void TestEffect::cleanupSwapChain()
{
    VkDevice device = graphicsDevice_->getLogicalDevice();
    for (int i = 0; i < DescriptorType::QUANTITY; ++i) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayouts_[i], nullptr);
    }
    vkDestroyDescriptorPool(device, descriptorPool_, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

